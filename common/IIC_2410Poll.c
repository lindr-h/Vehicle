#include <string.h>
#include "2410addr.h"
#include "IIC_def.h"

static unsigned char _iicData[256];
static volatile int _iicDataCount;
static volatile int _iicStatus;
static volatile int _iicMode;
static int _iicPt;

//===================================================================
//  S3C2410 IIC configuration
//  GPE15=IICSDA, GPE14=IICSCL
//  Non-Interrupt mode for IIC block
//=================================================================== 

void IIC_write(unsigned int addr ,unsigned char data)
{
	
    unsigned int save_E,save_PE;
//    unsigned int save_MPLLCON;
//    save_MPLLCON = rMPLLCON;
 //   rMPLLCON = 0xa1033;
    save_E   = rGPECON;
    save_PE  = rGPEUP;
    rGPEUP  |= 0xc000;                  //Pull-up disable
    rGPECON = (rGPECON|0xf0000000)&0xafffffff;           //GPE15:IICSDA , GPE14:IICSCL   
    //Enable ACK, Prescaler IICCLK=PCLK/512, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
    //24C02N 1.8-5.5V  100KHZ--400kHZ
    //PCLK=50.7MHZ   IICCLK=50.7/512=99KHZ
    rIICCON  = (1<<7) | (0<<6) | (1<<5) | (0xf);//0xef
    rIICADD  = 0xa0;                    
    rIICSTAT = 0x10;  //IIC bus data output enable(Tx/Rx)
    Wr24C02(0xa0,addr,data);
    
    rGPEUP  = save_PE;
    rGPECON = save_E;
 //   rMPLLCON = save_MPLLCON;

}

void IIC_read(unsigned int addr ,unsigned char* pdata)
{
    unsigned int save_E,save_PE;
//   unsigned int save_MPLLCON;
//    save_MPLLCON = rMPLLCON;

//    rMPLLCON = 0xa1033;
    save_E   = rGPECON;
    save_PE  = rGPEUP;
    rGPEUP  |= 0xc000;                  //Pull-up disable
    rGPECON = (rGPECON|0xf0000000)&0xafffffff;           //GPE15:IICSDA , GPE14:IICSCL   
    //Enable ACK, Prescaler IICCLK=PCLK/512, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
    //24C02N 1.8-5.5V  100KHZ--400kHZ
    //PCLK=50.7MHZ   IICCLK=50.7/512=99KHZ
    rIICCON  = (1<<7) | (0<<6) | (1<<5) | (0xf);//0xef
    rIICADD  = 0xa0;                    
    rIICSTAT = 0x10;  //IIC bus data output enable(Tx/Rx)
    Rd24C02(0xa0,addr,pdata);
    
    rGPEUP  = save_PE;
    rGPECON = save_E;
 //   rMPLLCON = save_MPLLCON;
}

//**********************[ DelayTime ]**************************************
void DelayTime(int time)
{
    int i;
    for(i=0;i<1000;i++);
    return;
}
//*******************************************************************


//**************[ Wr24C02]*****************************************
void Wr24C02(unsigned int slvAddr,unsigned int addr,unsigned char data)
{
    _iicMode      = WRDATA;
    _iicPt        = 0;
    _iicData[0]   = (unsigned char)addr;
    _iicData[1]   = data;
    _iicDataCount = 2;
    
    rIICDS        = slvAddr;            //0xa0
      //Master Tx mode, Start(Write), IIC-bus data output enable
      //Bus arbitration sucessful, Address as slave status flag Cleared,
      //Address zero status flag cleared, Last received bit is 0
    rIICSTAT      = 0xf0;      
      //Clearing the pending bit isn't needed because the pending bit has been cleared.
    while(_iicDataCount!=-1)
       Run_IicPoll();
  
    _iicMode = POLLACK;

    while(1)
    {
        rIICDS     = slvAddr;
        _iicStatus = 0x100;             //To check if _iicStatus is changed 
        rIICSTAT   = 0xf0;              //Master Tx, Start, Output Enable, Sucessful, Cleared, Cleared, 0
        rIICCON    = 0xaf;              //Resumes IIC operation. 
        while(_iicStatus==0x100)  
            Run_IicPoll();
              
        if(!(_iicStatus & 0x1))
            break;                      //When ACK is received
    }
    rIICSTAT = 0xd0;                    //Master Tx condition, Stop(Write), Output Enable
    rIICCON  = 0xaf;                    //Resumes IIC operation. 
    DelayTime(1);                           //Wait until stop condtion is in effect.
      //Write is completed.
}


        
//************************[ Rd24C02]********************************

void Rd24C02(unsigned int slvAddr,unsigned int addr,unsigned char *data)
{
    _iicMode      = SETRDADDR;
    _iicPt        = 0;
    _iicData[0]   = (unsigned char)addr;
    _iicDataCount = 1;

    rIICDS   = slvAddr;
    rIICSTAT = 0xf0;                    //MasTx,Start  
    //Clearing the pending bit isn't needed because the pending bit has been cleared.
    while(_iicDataCount!=-1)
        Run_IicPoll();

    _iicMode      = RDDATA;
    _iicPt        = 0;
    _iicDataCount = 1;
    
    rIICDS   = slvAddr;
    rIICSTAT = 0xb0;                    //Master Rx,Start
    rIICCON  = 0xaf;                    //Resumes IIC operation.   
    while(_iicDataCount!=-1)
        Run_IicPoll();

    *data = _iicData[1];
}


    
//**********************[Run_IicPoll]**************************************
void Run_IicPoll(void)
{
    if(rIICCON & 0x10)                  //Tx/Rx Interrupt Enable
       IicPoll();
}       
    
//**********************[IicPoll ]**************************************
void IicPoll(void)
{
    unsigned int iicSt,i;
    
    iicSt = rIICSTAT; 
   if(iicSt & 0x8){}                   //When bus arbitration is failed.
   if(iicSt & 0x4){}                   //When a slave address is matched with IICADD
   if(iicSt & 0x2){}                   //When a slave address is 0000000b
   if(iicSt & 0x1){}                   //When ACK isn't received

    switch(_iicMode)
    {
        case POLLACK:
            _iicStatus = iicSt;
            break;

        case RDDATA:
            if((_iicDataCount--)==0)
            {
                _iicData[_iicPt++] = rIICDS;
            
                rIICSTAT = 0x90;                //Stop MasRx condition 
                rIICCON  = 0xaf;                //Resumes IIC operation.
                DelayTime(1);                       //Wait until stop condtion is in effect.
                                                //Too long time... 
                                                //The pending bit will not be set after issuing stop condition.
                break;    
            }      
            _iicData[_iicPt++] = rIICDS;
                        //The last data has to be read with no ack.
            if((_iicDataCount)==0)
                rIICCON = 0x2f;                 //Resumes IIC operation with NOACK.  
            else 
                rIICCON = 0xaf;                 //Resumes IIC operation with ACK
            break;

        case WRDATA:
            if((_iicDataCount--)==0)
            {
                rIICSTAT = 0xd0;                //stop MasTx condition 
                rIICCON  = 0xaf;                //resumes IIC operation.
                DelayTime(1);                       //wait until stop condtion is in effect.
                       //The pending bit will not be set after issuing stop condition.
                break;    
            }
            rIICDS = _iicData[_iicPt++];        //_iicData[0] has dummy.
            for(i=0;i<10;i++);                  //for setup time until rising edge of IICSCL
            rIICCON = 0xaf;                     //resumes IIC operation.
            break;

        case SETRDADDR:

            if((_iicDataCount--)==0)
            {
                break;                  //IIC operation is stopped because of IICCON[4]    
            }
            rIICDS = _iicData[_iicPt++];
            for(i=0;i<10;i++);          //for setup time until rising edge of IICSCL
            rIICCON = 0xaf;             //resumes IIC operation.
            break;

        default:
            break;      
    }
}

