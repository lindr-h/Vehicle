#include "2410addr.h"
#include "spi.h"

#define SPIDONTCARE (0xff)



/* init as SPI-Master */
void spi_init(void)
{
    int i;
    rGPECON&=0xf03fffff;
    rGPECON|=((1<<26)|(1<<24)|(0<<22));
    
    rGPGCON&=0xffffffcf;
    rGPGCON|=(1<<4);

    rGPEUP&=~((0x7)<<11);
    rGPGUP&=~((0x1)<<2);
    
    
    rSPPRE0=(PCLK/(2*ucSpiBaud))-1;
	rSPCON0=(0<<5)|(1<<4)|(1<<3)|(0<<2)|(0<<1)|(0<<0);			//polling mode,enable ENSCK,master,CPOL=0,CPHA=0,normal mode	    
	rSPPIN0=(0<<2)|(1<<1)|(0<<0);								//dis-ENMUL,SBO,release	
    rGPGDAT &=~(0x04);											//nss=low
    
   for(i=0;i<10;i++)
      rSPTDAT0 = 0xff;
}


unsigned char spi_write(unsigned char data)
{
	// set data to send into SPI data register
	rSPTDAT0 = data;
	// Wait for transmission complete 
	while(!(rSPSTA0 & 0x1));
	// return data read from SPI
	return rSPTDAT0;
}

unsigned char spi_read(void)
{    
   rSPTDAT0 = SPIDONTCARE;
    while(!(rSPSTA0 & 0x1));
    
	return rSPRDAT0;
}



void WriteSPI(unsigned char DataOut)
{
  unsigned char n;
  
  rGPEDAT&=~(0x02000);   //  mSCK = 0;	Make sure clock is low

  
  for(n=0; n<8; n++)
  {
    rGPEDAT&=~(0x02000);   //  mSCK = 0;	Make sure clock is low
    if((DataOut << n) & 0x80)  //Clock a 1 0xFE
      rGPEDAT |= 0x1000;
    else                        //Clock a 0
      rGPEDAT &=~(0x01000);

    rGPEDAT|=(0x02000);	//  mSCK = 1;	Make sure clock is high
  }
}

/********************************************************************
*     Function Name:    ReadSPI                                     *
*     Return Value:     DataIn value                                *
*     Parameters:       void                                        *
*     Description:      Read single byte from SPI bus.              *
*                       Note: Clock comes in low and slave is       *
*                       already driving data.                       *
********************************************************************/
unsigned char ReadSPI( void )
{
  unsigned char n, DataIn = 0;
  
  rGPEDAT&=~(0x02000);   //  mSCK = 0;	Make sure clock is low
  
  for(n=0; n<7; n++)
  {
    rGPEDAT|=(0x02000);	//  mSCK = 1;	Make sure clock is high
    DataIn = DataIn + ((rGPEDAT>>11)&(0x1));
    DataIn = (DataIn << 1);
    rGPEDAT&=~(0x02000);   //  mSCK = 0;	Make sure clock is low
  }

   //last bit
   rGPEDAT|=0x2000;	//  mSCK = 1;	Make sure clock is high
   DataIn = DataIn + ((rGPEDAT>>11)&(0x1));
   rGPEDAT&=~(0x02000);   //  mSCK = 0;	Make sure clock is low
   return DataIn;
}


//-------------------------------------------------------------------------
//  SPIByteWrite()
//-------------------------------------------------------------------------
void SPIByteWrite(unsigned char addr, unsigned char value )
{
  rGPGDAT &=0xFFFB;		//nss=low
  WriteSPI(CAN_WRITE);
  WriteSPI(addr);
  WriteSPI(value);
  rGPGDAT |=0x4;        //nss=high
}


//-----------------------------------------------------------------------------
//  SPIByteRead()
//-----------------------------------------------------------------------------
unsigned char SPIByteRead(unsigned char addr)
{
  unsigned char tempdata;
  rGPGDAT &=0xFFFB;		//nss=low  
  WriteSPI(CAN_READ);
  WriteSPI(addr);
  tempdata = ReadSPI();
  rGPGDAT |=0x4;        //nss=high
  return tempdata;
}


//-----------------------------------------------------------------------------
//  SPIReset()
//-----------------------------------------------------------------------------
void SPIReset(void)
{
  rGPGDAT &=0xFFFB;		//nss=low
  WriteSPI(CAN_RESET);
  rGPGDAT |=0x4;        //nss=high
}

