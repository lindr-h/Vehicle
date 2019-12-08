#include "2410addr.h"
#include "2410lib.h"
#include "def.h"
#include "2410iis.h"
#include "INT.h"

void ChangeDMA2(void);
void IIS_PortSetting(void);
void _WrL3Addr(U8 data);
void _WrL3Data(U8 data,int halt);
void __irq DMA2_Done(void);
void __irq DMA2_Rec_Done(void);
void __irq RxInt(void);

#define L3C (1<<4)            //GPB4 = L3CLOCK
#define L3D (1<<3)            //GPB3 = L3DATA
#define L3M (1<<2)            //GPB2 = L3MODE

#define PLAY    0
#define RECORD  1
#define REC_LEN 0x50000       //327,680 Bytes

unsigned char *Buf,*_temp;
unsigned short *rec_buf;

volatile unsigned int size = 0;
volatile unsigned int   fs = 0;
volatile char    which_Buf = 1;
volatile char     Rec_Done = 0;
volatile char         mute = 1;

//------------------------------------------------------------------------------
//      SMDK2410 IIS Configuration
// GPB4 = L3CLOCK, GPB3 = L3DATA, GPB2 = L3MODE
// GPE4 = I2SSDO,  GPE3 = I2SSDI, GPE2 = CDCLK, GPE1 = I2SSCLK, GPE0 = I2SLRCK  
//------------------------------------------------------------------------------

//*********************[Iis_Tx] *********************************
void Iis_Tx(void)
{
    unsigned int save_B, save_E, save_PB, save_PE,i;
 
    ChangeClockDivider(1,1);        //1:2:4
    ChangeMPllValue(0x96,0x5,0x1);  //FCLK=135428571Hz, PCLK=3.385714MHz 
    save_B  = rGPBCON;       
    save_E  = rGPECON;       
    save_PB = rGPBUP;
    save_PE = rGPEUP;

    IIS_PortSetting();
  
  //  pISR_DMA2  = (unsigned)DMA2_Done;
    
   // rINTMSK    = ~(BIT_DMA2);   

      //Non-cacheable area = 0x31000000 ~ 0x33feffff
    Buf=(unsigned char *)WavBaseAddr;
    /*
    size = *(Buf+4) | *(Buf + 5)<<8 | *(Buf + 6)<<16 | *(Buf + 7)<<24;
    size = *(Buf + 0x28) | *(Buf + 0x29)<<8 | *(Buf + 0x2a)<<16 | *(Buf + 0x2b)<<24;
    size = (size>>1)<<1;
    fs   = *(Buf + 0x18) | *(Buf + 0x19)<<8 | *(Buf + 0x1a)<<16 | *(Buf + 0x1b)<<24;

    Init1341(PLAY);

      //DMA2 Initialize
    rDISRC2  = (int)(Buf + 0x30);               //0x31000030~(Remove header)      
    rDISRCC2 = (0<<1) + (0<<0);                 //The source is in the system bus(AHB), Increment      
    rDIDST2  = ((U32)IISFIFO);                  //IISFIFO    
    rDIDSTC2 = (1<<1) + (1<<0);                 //The destination is in the peripheral bus(APB), Fixed  
    rDCON2   = (1<<31)+(0<<30)+(0<<29)+(0<<28)+(0<<27)+(0<<24)+(1<<23)+(1<<22)+(1<<20)+(size/4);
      //Handshake, sync PCLK, TC int disable, single tx, single service, I2SSDO, I2S Rx request, 
      //noauto-reload, half-word,size/4.
    rDMASKTRIG2 = (0<<2) + (1<<1) + (0<<0);          //no-stop[2], DMA2 channel on[1], No-sw trigger[0] 

      //IIS Initialize
    if(fs==44100)               //11.2896MHz(256fs)
    {
        rIISPSR = (2<<5) + 2;   //Prescaler A,B=2 <- FCLK 135.4752MHz(1:2:4)
    }
    else                        //fs=22050, 5.6448MHz(256fs)
    {
        rIISPSR = (5<<5) + 5;   //Prescaler A,B=5 <- FCLK 135.4752MHz(1:2:4)     
    }
    rIISCON = (1<<5) + (1<<2) + (1<<1);         //Tx DMA enable[5], Rx idle[2], Prescaler enable[1]
      //Master mode[8],Tx mode[7:6],Low for Left Channel[5],IIS format[4],16bit ch.[3],CDCLK 256fs[2],IISCLK 32fs[1:0]
    rIISMOD = (0<<8) + (2<<6) + (0<<5) + (0<<4) + (1<<3) + (0<<2) + (1<<0);  
    rIISFCON = (1<<15) + (1<<13);        //Tx DMA,Tx FIFO --> start piling....
    
      //IIS Tx Start
    rIISCON |= 0x1;             //IIS Interface start
    while(i++<100);
    while((rDSTAT2 & 0xfffff)!=0x0)
    {
        ;;
    } 
   
    Delay(10);                   //For end of H/W Tx
    rIISCON      = 0x0;          //IIS Interface stop
    rDMASKTRIG2  = (1<<2);       //DMA2 stop
    rIISFCON     = 0x0;          //For FIFO flush
    
    size = 0;

    rGPBCON = save_B;
    rGPECON = save_E;
    rGPBUP  = save_PB;
    rGPEUP  = save_PE;

    rINTMSK |= (BIT_DMA2 );    
 //   ChangeMPllValue(0xa1,0x3,0x1);    // FCLK=202.8MHz
    
    */
    size = REC_LEN * 2;

    Init1341(PLAY);

    ISRVector[19] = DMA2_Done;
    rINTMSK   = ~(BIT_DMA2 );

      //DMA2 Initialize
    rDISRCC2 = (0<<1) + (0<<0);                         //AHB, Increment
    rDISRC2  = (int)rec_buf;                            //0x31000000
    rDIDSTC2 = (1<<1) + (1<<0);                         //APB, Fixed
    rDIDST2  = ((U32)IISFIFO);                          //IISFIFO
    rDCON2   = ((unsigned)1<<31)+(1<<23)+(1<<22)+(1<<20)+(size/2);
      //Handshake, sync PCLK, TC int, single tx, single service, I2SSDO, I2S request, 
      //Auto-reload, half-word, size/2
    rDMASKTRIG2 = (0<<2)+(1<<1)+0;    //No-stop, DMA2 channel on, No-sw trigger 

      //IIS Initialize
      //Master,Tx,L-ch=low,iis,16bit ch.,CDCLK=256fs,IISCLK=32fs
    rIISMOD = (0<<8) + (2<<6) + (0<<5) + (0<<4) + (1<<3) + (0<<2) + (1<<0);      
    rIISCON = (1<<5)+(0<<4)+(0<<3)+(1<<2)+(1<<1);
      //Tx DMA enable,Tx DMA disable,Tx not idle,Rx idle,prescaler enable,stop
    rIISFCON = (1<<15) + (1<<13);       //Tx DMA,Tx FIFO --> start piling....


    rIISCON |= 0x1;                   //IIS Tx Start
  
    while(i++<10000);
    while((rDSTAT2 & 0xfffff)!=0x0)
    {
        ;;
    } 
   
   
      //IIS Tx Stop
    Delay(10);               //For end of H/W Tx
    rIISCON     = 0x0;       //IIS stop
    rDMASKTRIG2 = (1<<2);    //DMA2 stop
    rIISFCON    = 0x0;       //For FIFO flush

    size = 0;

    rGPBCON = save_B;
    rGPECON = save_E;
    rGPBUP  = save_PB;
    rGPEUP  = save_PE;

    rINTMSK |= BIT_DMA2;
    

    ChangeMPllValue(0xa1,0x3,0x1);              // FCLK=202.8MHz
    mute = 1;
}


//**************** [ Record_Iis ] ***************************************
void Record_Iis(void)
{
    unsigned int save_B, save_E, save_PB, save_PE,i;

    ChangeClockDivider(1,1);        //1:2:4
    ChangeMPllValue(0x96,0x5,0x1);  //FCLK=135428571Hz, PCLK=3.385714MHz
    
    save_B  = rGPBCON;       
    save_E  = rGPECON;       
    save_PB = rGPBUP;
    save_PE = rGPEUP;

    IIS_PortSetting();
        
      //--- Record Buf initialize
      //Non-cacheable area = 0x31000000 ~ 0x33feffff
    rec_buf = (unsigned short*)WavBaseAddr;

    ISRVector[19]  = DMA2_Rec_Done;
    //pISR_EINT0 = (unsigned)Muting;
 
   // rINTSUBMSK=~(BIT_SUB_TC);
    rINTMSK = ~(BIT_DMA2);
    
    Init1341(RECORD);

      //--- DMA2 Initialize
    rDISRCC2 = (1<<1) + (1<<0);                         //APB, Fix  
    rDISRC2  = ((U32)IISFIFO);                          //IISFIFO
    rDIDSTC2 = (0<<1) + (0<<0);                         //PHB, Increment
    rDIDST2  = (int)rec_buf;                            //0x31000000 ~
    rDCON2   = ((unsigned)1<<31)+(1<<24)+(1<<23)+(1<<22)+(1<<20)+REC_LEN;
      //Handshake, sync PCLK, TC int, single tx, single service, I2SSDI, I2S Rx request, 
      //Off-reload, half-word, 0x50000 half word.
    rDMASKTRIG2 = (0<<2) + (1<<1) + 0;    //No-stop, DMA2 channel on, No-sw trigger

      //IIS Initialize
      //Master,Rx,L-ch=low,IIS,16bit ch,CDCLK=256fs,IISCLK=32fs
    rIISMOD = (0<<8) + (1<<6) + (0<<5) + (0<<4) + (1<<3) + (0<<2) + (1<<0);
    rIISPSR = (4<<5) + 4; //Prescaler_A/B=4 <- FCLK 112.896MHz(1:2:2) ,11.2896MHz(256fs),44.1KHz
    rIISCON = (0<<5) + (1<<4) + (1<<3) + (0<<2) + (1<<1);
      //Tx DMA disable,Rx DMA enable,Tx idle,Rx not idle,prescaler enable,stop
    rIISFCON = (1<<14) + (1<<12);     //Rx DMA,Rx FIFO --> start piling....


      //Rx start
    rIISCON |= 0x1;

    //while(!Rec_Done);
     while(i++<10000);
    while((rDSTAT2 & 0xfffff)!=0x0)
    {
        ;;
    } 
   
   
    rINTMSK|= BIT_DMA2;
  //  Rec_Done = 0;

      //IIS Stop
    Delay(10);                          //For end of H/W Rx
    rIISCON     = 0x0;                  //IIS stop
    rDMASKTRIG2 = (1<<2);               //DMA2 stop
    rIISFCON    = 0x0;                  //For FIFO flush
/*
    size = REC_LEN * 2;

    Init1341(PLAY);

    pISR_DMA2 = (unsigned)DMA2_Done;
    rINTMSK   = ~(BIT_DMA2 );

      //DMA2 Initialize
    rDISRCC2 = (0<<1) + (0<<0);                         //AHB, Increment
    rDISRC2  = (int)rec_buf;                            //0x31000000
    rDIDSTC2 = (1<<1) + (1<<0);                         //APB, Fixed
    rDIDST2  = ((U32)IISFIFO);                          //IISFIFO
    rDCON2   = (1<<31)+(0<<30)+(0<<29)+(0<<28)+(0<<27)+(0<<24)+(1<<23)+(1<<22)+(1<<20)+(size/2);
      //Handshake, sync PCLK, TC int, single tx, single service, I2SSDO, I2S request, 
      //Auto-reload, half-word, size/2
    rDMASKTRIG2 = (0<<2)+(1<<1)+0;    //No-stop, DMA2 channel on, No-sw trigger 

      //IIS Initialize
      //Master,Tx,L-ch=low,iis,16bit ch.,CDCLK=256fs,IISCLK=32fs
    rIISMOD = (0<<8) + (2<<6) + (0<<5) + (0<<4) + (1<<3) + (0<<2) + (1<<0);      
    rIISCON = (1<<5)+(0<<4)+(0<<3)+(1<<2)+(1<<1);
      //Tx DMA enable,Tx DMA disable,Tx not idle,Rx idle,prescaler enable,stop
    rIISFCON = (1<<15) + (1<<13);       //Tx DMA,Tx FIFO --> start piling....


    rIISCON |= 0x1;                   //IIS Tx Start
  
    while(i++<10000);
    while((rDSTAT2 & 0xfffff)!=0x0)
    {
        ;;
    } 
   
   
      //IIS Tx Stop
    Delay(10);               //For end of H/W Tx
    rIISCON     = 0x0;       //IIS stop
    rDMASKTRIG2 = (1<<2);    //DMA2 stop
    rIISFCON    = 0x0;       //For FIFO flush

    size = 0;

    rGPBCON = save_B;
    rGPECON = save_E;
    rGPBUP  = save_PB;
    rGPEUP  = save_PE;

    rINTMSK |= BIT_DMA2;
    

    ChangeMPllValue(0xa1,0x3,0x1);              // FCLK=202.8MHz
    mute = 1;
   */ 
}


//******************[ Init1341 ]**************************************
void Init1341(char mode)
{
    //Port Initialize
//----------------------------------------------------------
//   PORT B GROUP
//Ports  :   GPB4    GPB3   GPB2  
//Signal :  L3CLOCK L3DATA L3MODE
//Setting:   OUTPUT OUTPUT OUTPUT 
//           [9:8]   [7:6}  [5:4]
//Binary :     01  ,   01    01 
//----------------------------------------------------------    
    rGPBDAT = rGPBDAT & ~(L3M|L3C|L3D) |(L3M|L3C); //Start condition : L3M=H, L3C=H
    rGPBUP  = rGPBUP  & ~(0x7<<2) |(0x7<<2);       //The pull up function is disabled GPB[4:2] 1 1100    
    rGPBCON = rGPBCON & ~(0x3f<<4) |(0x15<<4);     //GPB[4:2]=Output(L3CLOCK):Output(L3DATA):Output(L3MODE)

      //L3 Interface
    _WrL3Addr(0x14 + 2);     //STATUS (000101xx+10)
    _WrL3Data(0x60,0);       //0,1,10,000,0 : Reset,256fs,no DCfilter,iis

    _WrL3Addr(0x14 + 2);     //STATUS (000101xx+10)
    _WrL3Data(0x20,0);       //0,0,10,000,0 : No reset,256fs,no DCfilter,iis
    
    _WrL3Addr(0x14 + 2);     //STATUS (000101xx+10)
    _WrL3Data(0x81,0);       //1,0,0,0,0,0,01 : OGS=0,IGS=0,ADC_NI,DAC_NI,sngl speed,AoffDon
    
      //record
    if(mode)
    {
        _WrL3Addr(0x14 + 2);     //STATUS (000101xx+10)
        _WrL3Data(0xa2,0);       //1,0,1,0,0,0,10 : OGS=0,IGS=1,ADC_NI,DAC_NI,sngl speed,AonDoff

        _WrL3Addr(0x14 + 0);     //DATA0 (000101xx+00)
        _WrL3Data(0xc2,0);       //11000,010  : DATA0, Extended addr(010) 
        _WrL3Data(0x4d,0);       //010,011,01 : DATA0, MS=9dB, Ch1=on Ch2=off, 
    }
}

//===================================================================
void ChangeDMA2(void)
{
    if(which_Buf)
    {
        rDISRCC2 = (0<<1) + (0<<0);                         //AHB, Increment
        rDISRC2  = (int)(Buf + 0x30);                       //0x31000030~(Remove header)
    }
    else
    {
        rDISRCC2 = (0<<1) + (0<<0);                         //AHB, Increment
        rDISRC2  = (int)(Buf + 0x30+(size/2));              //0x31000030 + size/2~
    }
}

//===================================================================
void IIS_PortSetting(void)
{
//----------------------------------------------------------
//   PORT B GROUP
//Ports  :   GPB4    GPB3   GPB2  
//Signal :  L3CLOCK L3DATA L3MODE
//Setting:   OUTPUT OUTPUT OUTPUT 
//           [9:8]   [7:6}  [5:4]
//Binary :     01  ,   01    01 
//----------------------------------------------------------    
    rGPBUP  = rGPBUP  & ~(0x7<<2) | (0x7<<2);   //The pull up function is disabled GPB[4:2] 1 1100    
    rGPBCON = rGPBCON & ~(0x3f<<4) | (0x15<<4); //GPB[4:2]=Output(L3CLOCK):Output(L3DATA):Output(L3MODE)

//----------------------------------------------------------
//   PORT E GROUP
//Ports  :  GPE4    GPE3   GPE2  GPE1    GPE0 
//Signal : I2SSDO  I2SSDI CDCLK I2SSCLK I2SLRCK 
//Binary :   10  ,   10     10 ,  10      10    
//----------------------------------------------------------
    rGPEUP  = rGPEUP  & ~(0x1f)  | 0x1f;    //The pull up function is disabled GPE[4:0] 1 1111
    rGPECON = rGPECON & ~(0x3ff) | 0x2aa;   //GPE[4:0]=I2SSDO:I2SSDI:CDCLK:I2SSCLK:I2SLRCK

    rGPFUP   = ((rGPFUP   & ~(1<<0)) | (1<<0));     //GPF0
    rGPFCON  = ((rGPFCON  & ~(3<<0)) | (1<<1));     //GPF0=EINT0    
    
    rEXTINT0 = ((rEXTINT0 & ~(7<<0)) | (2<<0));     //EINT0=falling edge triggered  
}

//===================================================================
void _WrL3Addr(U8 data)
{       
    S32 i,j;

    rGPBDAT  = rGPBDAT & ~(L3D | L3M | L3C) | L3C;      //L3D=L, L3M=L(in address mode), L3C=H

    for(j=0;j<4;j++);                   //tsu(L3) > 190ns

      //GPB[4:2]=L3C:L3D:L3M
    for(i=0;i<8;i++)                    //LSB first
    {
        if(data & 0x1)                  //If data's LSB is 'H'
        {
            rGPBDAT &= ~L3C;            //L3C=L
            rGPBDAT |= L3D;             //L3D=H             
            
            for(j=0;j<4;j++);           //tcy(L3) > 500ns
            
            rGPBDAT |= L3C;             //L3C=H
            rGPBDAT |= L3D;             //L3D=H
            
            for(j=0;j<4;j++);           //tcy(L3) > 500ns
        }
        else                            //If data's LSB is 'L'
        {
            rGPBDAT &= ~L3C;            //L3C=L
            rGPBDAT &= ~L3D;            //L3D=L
            
            for(j=0;j<4;j++);           //tcy(L3) > 500ns
            
            rGPBDAT |= L3C;             //L3C=H
            rGPBDAT &= ~L3D;            //L3D=L
            
            for(j=0;j<4;j++);           //tcy(L3) > 500ns            
        }
        data >>= 1;
    }
    rGPBDAT  = rGPBDAT & ~(L3D | L3M | L3C) | (L3C | L3M);       //L3M=H,L3C=H   
}

//===================================================================
void _WrL3Data(U8 data,int halt)
{
    S32 i,j;

    if(halt)
    {
        rGPBDAT  = rGPBDAT & ~(L3D | L3M | L3C) | L3C;   //L3C=H(while tstp, L3 interface halt condition)        
        for(j=0;j<4;j++);                                //tstp(L3) > 190ns
    }

    rGPBDAT  = rGPBDAT & ~(L3D | L3M | L3C) | (L3C | L3M);   //L3M=H(in data transfer mode)        
    for(j=0;j<4;j++);                                        //tsu(L3)D > 190ns

      //GPB[4:2]=L3C:L3D:L3M
    for(i=0;i<8;i++)
    {
        if(data & 0x1)                   //if data's LSB is 'H'
        {
           rGPBDAT &= ~L3C;              //L3C=L
           rGPBDAT |= L3D;               //L3D=H
           
           for(j=0;j<4;j++);             //tcy(L3) > 500ns
           
           rGPBDAT |= (L3C | L3D);       //L3C=H,L3D=H
           
           for(j=0;j<4;j++);             //tcy(L3) > 500ns
        }
        else                             //If data's LSB is 'L'
        {
           rGPBDAT &= ~L3C;              //L3C=L
           rGPBDAT &= ~L3D;              //L3D=L
           for(j=0;j<4;j++);             //tcy(L3) > 500ns
           rGPBDAT |= L3C;               //L3C=H
           rGPBDAT &= ~L3D;              //L3D=L
           for(j=0;j<4;j++);             //tcy(L3) > 500ns
        }
        data >>= 1;                      //For check next bit
    }
    rGPBDAT  = rGPBDAT & ~(L3D | L3M | L3C) | (L3C | L3M);    //L3M=H,L3C=H
}

//===================================================================
void __irq DMA2_Done(void)
{
    rSRCPND = BIT_DMA2;                 //Clear pending bit
    rINTPND = BIT_DMA2;

//    WrUTXH0('@');
    if(!which_Buf)
        which_Buf = 1;
    else
        which_Buf = 0;
}

//===================================================================
void __irq DMA2_Rec_Done(void)
{
    rSRCPND = BIT_DMA2;                 //Clear pending bit
    rINTPND = BIT_DMA2;

    Rec_Done = 1;
}

//===================================================================
void __irq RxInt(void)
{
    rSUBSRCPND = BIT_SUB_RXD0;          //Clear pending bit (Requested)
   
    ClearPending(BIT_UART0);

    *_temp++ = RdURXH0(); 
}

//===================================================================
/* void __irq Muting(void)
{
    rSRCPND = BIT_EINT0;                //Clear pending bit
    rINTPND = BIT_EINT0;
    rINTPND;

    if(mute)    //Mute
    {
        _WrL3Addr(0x14 + 0);            //DATA0 (000101xx+00)
        _WrL3Data(0xa4,0);              //10,1,00,1,00 : after, no de-emp, mute, flat 
        mute = 0;
        Uart_Printf("=== Mute ===\n");
    }
    else        //No mute
    {
        _WrL3Addr(0x14 + 0);            //DATA0 (000101xx+00)
        _WrL3Data(0xa0,0);              //10,1,00,0,00 : after, no de-emp, no mute, flat 
        mute = 1;
        Uart_Printf("=== No Mute ===\n");
    }
}
*/

void playwav(void)
{
//	unsigned int save_B, save_E, save_PB, save_PE;

//	save_B  = rGPBCON;       
//	save_E  = rGPECON;       
//	save_PB = rGPBUP;
//	save_PE = rGPEUP;
	
//	ChangeClockDivider(1,1);		// 1:2:4
//	ChangeMPllValue(0x96,0x5,0x1);	// FCLK=135428571Hz, PCLK=3.385714MHz
	
	size = 0;
	IIS_PortSetting();
	
//	ISRVector[19] = DMA2_Done;
	
//	rINTSUBMSK &= ~(BIT_SUB_TC);
//	rINTMSK &= ~(BIT_DMA2 | BIT_ADC);   
	
	// Non-cacheable area = 0x31000000 ~ 0x33feffff
	Buf = (unsigned char *)WavBaseAddr;													// WAV文件所在地址
//	size = *(Buf+4) | *(Buf + 5)<<8 | *(Buf + 6)<<16 | *(Buf + 7)<<24;					// 文件长度
	size = *(Buf + 0x28) | *(Buf + 0x29)<<8 | *(Buf + 0x2a)<<16 | *(Buf + 0x2b)<<24;	// 语音数据的长度
//	size = (size>>1)<<1;																// 把0位置0(因为WAV文件的语音数据是偶数的方式储存)
	fs   = *(Buf + 0x18) | *(Buf + 0x19)<<8 | *(Buf + 0x1a)<<16 | *(Buf + 0x1b)<<24;	// 采样率（每秒样本数），表示每个通道的播放速度

	Init1341(PLAY);

	// DMA2 Initialize
	rDISRC2  = (int)(Buf + 0x30);				// 0x31000030~(Remove header)      
	rDISRCC2 = (0<<1) + (0<<0);					// The source is in the system bus(AHB), Increment      
	rDIDST2  = ((U32)IISFIFO);					// IISFIFO    
	rDIDSTC2 = (1<<1) + (1<<0);					// The destination is in the peripheral bus(APB), Fixed  
	rDCON2   = ((unsigned)1<<31)+(0<<30)+(0<<29)+(0<<28)+(0<<27)+(0<<24)+(1<<23)+(1<<22)+(1<<20)+(size/2);
	// Handshake, sync PCLK, TC int disable, single tx, single service, I2SSDO, I2S Rx request, 
	// noauto-reload, half-word,size/4.
	rDMASKTRIG2 = (0<<2) + (1<<1) + (0<<0);		// no-stop[2], DMA2 channel on[1], No-sw trigger[0] 

	// IIS Initialize
	if(fs==44100)					// 11.2896MHz(256fs)
	{
		rIISPSR = (2<<5) + 2;		// Prescaler A,B=2 <- FCLK 135.4752MHz(1:2:4)
	}
	else							// fs=22050, 5.6448MHz(256fs)
	{
		// rIISPSR = (5<<5) + 5;	// Prescaler A,B=5 <- FCLK 135.4752MHz(1:2:4)
		// rIISPSR = 0x1ce;
		rIISPSR =(8<<5)+8;
	}
	rIISCON = (1<<5) + (1<<2) + (1<<1);		// Tx DMA enable[5], Rx idle[2], Prescaler enable[1]
	// Master mode[8],Tx mode[7:6],Low for Left Channel[5],IIS format[4],16bit ch.[3],CDCLK 256fs[2],IISCLK 32fs[1:0]
	rIISMOD = (0<<8) + (2<<6) + (0<<5) + (0<<4) + (1<<3) + (0<<2) + (1<<0);  
	rIISFCON = (1<<15) + (1<<13);			// Tx DMA,Tx FIFO --> start piling....
	
	//IIS Tx Start
	rIISCON |= 0x1;							// IIS Interface start
//	while(i++<100);
	while((rDSTAT2 & 0xfffff)!=0x0);
	
	Delay(30);						// For end of H/W Tx
	rIISCON      = 0x0;				// IIS Interface stop
	rDMASKTRIG2  = (1<<2);			// DMA2 stop
	rIISFCON     = 0x0;				// For FIFO flush
	
//	size = 0;

//	rGPBCON = save_B;
//	rGPECON = save_E;
//	rGPBUP  = save_PB;
//	rGPEUP  = save_PE;

//	rINTMSK |= (BIT_DMA2 );
	
//	ChangeClockDivider(0,0);		// 1:1:1
//	ChangeMPllValue(0x70,0x2,0x2);	// FCLK=202.8MHz
}

/***********************************************************************/

