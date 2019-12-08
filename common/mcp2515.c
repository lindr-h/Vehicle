/*!	\file		mcp2515.c
	\brief		mcp2515芯片操作实现文件
	\author		丁雷
	\version	0.1
*/


#include "spi.h"

#include "mcp2515.h"
#include "can.h"

// Buffer can be MCP_TXBUF_0 MCP_TXBUF_1 or MCP_TXBUF_2
void mcp2515_write_canMsg(unsigned char buffer_sidh_addr, CanMessage * msg)
{
    unsigned char mcp_addr, dlc;

	mcp_addr = buffer_sidh_addr;
	dlc = msg->dlc;
	
    mcp2515_setRegisterS(mcp_addr+5, &(msg->dta[0]), dlc );  // write data bytes
    mcp2515_write_can_id( mcp_addr, msg->extended_identifier,msg->identifier );  // write CAN id
    if ( msg->rtr == 1)  dlc |= MCP_RTR_MASK;  // if RTR set bit in byte
    mcp2515_setRegister( (mcp_addr+4), dlc );  // write the RTR and DLC
}

void mcp2515_read_canMsg(unsigned char buffer_sidh_addr,CanMessage * msg)
{

    unsigned char mcp_addr, ctrl;

	mcp_addr = buffer_sidh_addr;
	
    mcp2515_read_can_id(mcp_addr, &(msg->extended_identifier),&(msg->identifier) );
    
	ctrl = mcp2515_readRegister( mcp_addr-1 );
    msg->dlc = mcp2515_readRegister( mcp_addr+4 );
    
	if ((ctrl & 0x08)) {
        msg->rtr = 1;
    } else {
        msg->rtr = 0;
    }
    
	msg->dlc &= MCP_DLC_MASK;
    mcp2515_readRegisterS( mcp_addr+5, &(msg->dta[0]), msg->dlc );
}

void mcp2515_read_can_id(unsigned char mcp_addr,unsigned char * ext,unsigned int * can_id )
{ 
    char tbufdata[4];
	
    *ext = 0;
    *can_id = 0;
    
	mcp2515_readRegisterS( mcp_addr, tbufdata, 4 );
    
	*can_id = (tbufdata[MCP_SIDH]<<3) + (tbufdata[MCP_SIDL]>>5);
	
    if ( (tbufdata[MCP_SIDL] & MCP_TXB_EXIDE_M) ==  MCP_TXB_EXIDE_M ) {
		// extended id
        *can_id = (*can_id<<2) + (tbufdata[MCP_SIDL] & 0x03);
        *can_id <<= 16;
        *can_id = *can_id +(tbufdata[MCP_EID8]<<8) + tbufdata[MCP_EID0];
        *ext = 1;
    }
}

void mcp2515_readRegisterS(unsigned char address,char values[], unsigned char n)
{
	unsigned char i;
	
	MCP2515_SELECT();
	WriteSPI(MCP_READ);
	WriteSPI(address);
	// mcp2515 has auto-increment of address-pointer
	for (i=0; i<n; i++) {
		values[i] = ReadSPI();
	}
	MCP2515_UNSELECT();
}


void mcp2515_reset(void)
{
   
	MCP2515_SELECT();
	WriteSPI(MCP_RESET);
	MCP2515_UNSELECT();
}

unsigned char mcp2515_readRegister(const unsigned char address)
{
	unsigned char ret;
	
	MCP2515_SELECT();
	WriteSPI(MCP_READ);
	WriteSPI(address);
	ret = ReadSPI();
	MCP2515_UNSELECT();
	
	return ret;
}


void mcp2515_setRegister(const unsigned char address, const unsigned char value)
{
	MCP2515_SELECT();
	WriteSPI(MCP_WRITE);
	WriteSPI(address);
	WriteSPI(value);
	MCP2515_UNSELECT();
}


void mcp2515_modifyRegister(const unsigned char address, 
	const unsigned char mask, const unsigned char data)
{
	MCP2515_SELECT();
	WriteSPI(MCP_BITMOD);
	WriteSPI(address);
	WriteSPI(mask);
	WriteSPI(data);
	MCP2515_UNSELECT();
}

static unsigned char mcp2515_readXXStatus_helper(const unsigned char cmd)
{
	unsigned char i;
	
	MCP2515_SELECT();
	WriteSPI(cmd);
	i = ReadSPI();
	MCP2515_UNSELECT();
	
	return i;
}

	
unsigned char mcp2515_readStatus(void)
{
	return mcp2515_readXXStatus_helper(MCP_READ_STATUS);
}

unsigned char mcp2515_RXStatus(void)
{
	return mcp2515_readXXStatus_helper(MCP_RX_STATUS);
}

unsigned char mcp2515_setCANCTRL_Mode(const unsigned char newmode)
{
	unsigned char i;
	
	mcp2515_modifyRegister(MCP_CANCTRL, MODE_MASK, newmode);
	
	// verify as advised in datasheet
	i = mcp2515_readRegister(MCP_CANCTRL);
	i &= MODE_MASK;
	
	if ( i == newmode ) {
		return MCP2515_OK; 
	}
	else {
		return MCP2515_FAIL;
	}
}


unsigned char mcp2515_configRate(const unsigned char canSpeed)
{
	unsigned char set, cfg1, cfg2, cfg3;
	
	set = 0;
	
	switch (canSpeed) {
		case (CAN_125KBPS) :
			cfg1 = MCP_4MHz_125kBPS_CFG1 ;
			cfg2 = MCP_4MHz_125kBPS_CFG2 ;
			cfg3 = MCP_4MHz_125kBPS_CFG3 ;
			set = 1;
			break;
		case (CAN_20KBPS) :
			cfg1 = MCP_4MHz_20kBPS_CFG1 ;
			cfg2 = MCP_4MHz_20kBPS_CFG2 ;
			cfg3 = MCP_4MHz_20kBPS_CFG3 ;
			set = 1;
			break;
		default:
			set = 0;
			break;
	}
	
	if (set) {
		mcp2515_setRegister(MCP_CNF1, cfg1);
		mcp2515_setRegister(MCP_CNF2, cfg2);
		mcp2515_setRegister(MCP_CNF3, cfg3);
		return MCP2515_OK;
	}
	else {
		return MCP2515_FAIL;
	}
} 


/*
 ** Start the transmission from one of the tx buffers.
 */
// Buffer can be MCP_TXBUF_0 MCP_TXBUF_1 or MCP_TXBUF_2
void mcp2515_start_transmit(const unsigned char buffer_sidh_addr)
{
	// TXBnCTRL_addr = TXBnSIDH_addr - 1
	
	
	switch(buffer_sidh_addr)
	{
	   case 0x31: MCP2515_SELECT();
	              WriteSPI(0x81);
	              MCP2515_UNSELECT();
	              break;
	   case 0x41: MCP2515_SELECT();
	              WriteSPI(0x82);
	              MCP2515_UNSELECT();
	              break;
	   case 0x51: MCP2515_SELECT();
	              WriteSPI(0x84);
	              MCP2515_UNSELECT();
	              break;
	}
//    mcp2515_modifyRegister( buffer_sidh_addr-1 , MCP_TXB_TXREQ_M|MCP_TXB_TXP10_M|MCP_TXB_ABTF_M|MCP_TXB_MLOA_M|MCP_TXB_TXERR_M, 
//		MCP_TXB_TXREQ_M|MCP_TXB_TXP10_M|(0x00));
	
}

void mcp2515_setRegisterS(unsigned char address,char values[],unsigned char n)
{
	unsigned char i;
	
	MCP2515_SELECT();
	WriteSPI(MCP_WRITE);
	WriteSPI(address);
	// mcp2515 has auto-increment of address-pointer
	for (i=0; i<n; i++) {
		WriteSPI(values[i]);
	}
	MCP2515_UNSELECT();
}


void mcp2515_write_can_id( const unsigned char mcp_addr, 
	const unsigned char ext, const unsigned int can_id )
{
    unsigned int canid;
    char tbufdata[4];
	
    canid = (unsigned int)(can_id & 0x0FFFF);
    
	if ( ext == 1) {
        tbufdata[MCP_EID0] = (unsigned char) (canid & 0xFF);
        tbufdata[MCP_EID8] = (unsigned char) (canid / 256);
        canid = (unsigned int)( can_id / 0x10000L );
        tbufdata[MCP_SIDL] = (unsigned char) (canid & 0x03);
        tbufdata[MCP_SIDL] += (unsigned char) ((canid & 0x1C )*8);
        tbufdata[MCP_SIDL] |= MCP_TXB_EXIDE_M;
        tbufdata[MCP_SIDH] = (unsigned char) (canid / 32 );
    }
    else {
        tbufdata[MCP_SIDH] = (unsigned char) (canid>>3 );
        tbufdata[MCP_SIDL] = (unsigned char) ((canid & 0x07 )<<5);
        tbufdata[MCP_EID0] = 0;
        tbufdata[MCP_EID8] = 0;
    }
	mcp2515_setRegisterS( mcp_addr, tbufdata, 4 );
}

void mcp2515_initCANBuffers(void)
{
	unsigned char i, a1, a2, a3;
	
	// TODO: check why this is needed to receive extended 
	//   and standard frames
	// Mark all filter bits as don't care:
    mcp2515_write_can_id(MCP_RXM0SIDH, 0, 0);
    mcp2515_write_can_id(MCP_RXM1SIDH, 0, 0);
    // Anyway, set all filters to 0:
    mcp2515_write_can_id(MCP_RXF0SIDH, 1, 0); // RXB0: extended 
    mcp2515_write_can_id(MCP_RXF1SIDH, 0, 0); //       AND standard
    mcp2515_write_can_id(MCP_RXF2SIDH, 1, 0); // RXB1: extended 
    mcp2515_write_can_id(MCP_RXF3SIDH, 0, 0); //       AND standard
    mcp2515_write_can_id(MCP_RXF4SIDH, 0, 0);
    mcp2515_write_can_id(MCP_RXF5SIDH, 0, 0);
	
	// Clear, deactivate the three transmit buffers
	// TXBnCTRL -> TXBnD7
    a1 = MCP_TXB0CTRL;
	a2 = MCP_TXB1CTRL;
	a3 = MCP_TXB2CTRL;
    for (i = 0; i < 14; i++) { // in-buffer loop
		mcp2515_setRegister(a1, 0);
		mcp2515_setRegister(a2, 0);
		mcp2515_setRegister(a3, 0);
        a1++;
		a2++;
		a3++;
    }
	
    // and clear, deactivate the two receive buffers.
    mcp2515_setRegister(MCP_RXB0CTRL, 0);
    mcp2515_setRegister(MCP_RXB1CTRL, 0);
	mcp2515_setRegister(MCP_CANINTE,0xff);
}


// ---

unsigned char mcp2515_Init(const unsigned char canSpeed)
{
	unsigned char res;
	
	MCP2515_SELECT();
	
	mcp2515_reset();
	
	res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
	
	if ( res == MCP2515_FAIL ) return res;  /* function exit on error */
	
	res = mcp2515_configRate(canSpeed);
	
	
	mcp2515_initCANBuffers();
		
		// enable both receive-buffers to receive any message
		// and enable rollover
	mcp2515_modifyRegister(MCP_RXB0CTRL, 
			MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK, 
			MCP_RXB_RX_ANY | MCP_RXB_BUKT_MASK);
	mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK, 
			MCP_RXB_RX_ANY);
	
 
	mcp2515_modifyRegister(MCP_CANINTE,0x03,0x03);
	while(!(mcp2515_setCANCTRL_Mode(MODE_NORMAL) == MCP2515_OK));
	
	return res;
}