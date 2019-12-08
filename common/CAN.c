/*!	\file		CAN.c
	\brief		CAN收发数据实现文件
	\author		丁雷
	\version	0.1

	用户通过文件实现CAN总线的初始化，数据的发送，数据的中断接受。
*/
#include<stdlib.h>
#include<string.h>

#include "CAN.h"
#include "def.h"
#include "2410addr.h"
#include "option.h"
//#include "GUI.h"
#include "2410lib.h"
//#include "lcd.h"
#include "spi.h"
#include "mcp2515.h"
//#include "consol.h"
#include "spi.h"
#include "lcdlib.h"

/*!	\defgroup	CANDAT	CAN总线消息接受存放单元
*/
//@{
char messagebuf[100];	//!<字符串数据缓冲区
char RxMsgBuf[256];
int  RxMsgBufW;
int  RxMsgBufR;
//@}

/** CAN总线消息结构体初始化申请空间函数 
*  @param[in] msg CAN总线消息结构体
*/
void can_initMessageStruct(CanMessage* msg)
{
	memset(msg,0,sizeof(CanMessage));
}

void can_initBuf(void)
{
	memset(RxMsgBuf,0,sizeof(RxMsgBuf));
	RxMsgBufW = 0;
	RxMsgBufR = 0;
}

/** 芯片MCP2515申请空闲发送缓冲器函数 
*  @param[in] txbuf_n 发送缓冲器序号
*/
unsigned char mcp2515_getNextFreeTXBuf(unsigned char *txbuf_n)
{
	unsigned char res, i, ctrlval;
	unsigned char ctrlregs[MCP_N_TXBUFFERS] = { MCP_TXB0CTRL, MCP_TXB1CTRL, MCP_TXB2CTRL };
	
	res = MCP_ALLTXBUSY;
	*txbuf_n = 0x00;
	
	// check all 3 TX-Buffers
	for (i=0; i<MCP_N_TXBUFFERS; i++) {
		ctrlval = mcp2515_readRegister( ctrlregs[i] );
		if ( (ctrlval & MCP_TXB_TXREQ_M) == 0 ) {
			*txbuf_n = ctrlregs[i]+1; // return SIDH-address of Buffer
			res = MCP2515_OK;
			return res; /* ! function exit */
		}
	}
	
	return res;
}


/** CAN总线发送数据函数
*  @param[in] msg CAN总线消息结构体
*/
unsigned char can_sendMessage(CanMessage* msg)
{
	unsigned char res, txbuf_n;
	int i=0;
	//unsigned char timeout = 0;

	do {
		res = mcp2515_getNextFreeTXBuf(&txbuf_n); // info = addr.
		i++;
	} while (res == MCP_ALLTXBUSY && i<1000);

	
	if (res!=MCP_ALLTXBUSY) {
		mcp2515_write_canMsg( txbuf_n, msg);
		mcp2515_start_transmit( txbuf_n );
		return CAN_OK;
	}
	else {
		return CAN_FAILTX;
	}
}

/** 数据发送函数 
*  @param[in] value 要发送的数据
*  @param[in] n		要发送的数据的长度
*/
void CAN_Write(char * value,int n)
{
   CanMessage msg;
   can_initMessageStruct(&msg);
   msg.identifier = CANDEFAULTIDENT;
   msg.extended_identifier = CANDEFAULTIDENTEXT;
   msg.dlc = n;
   msg.rtr = 0;
   strncpy(msg.dta,value,n);
   	
   can_sendMessage(&msg);

}

/** CAN总线读数据函数 
*/
void CAN_Read()
{
    
    unsigned char stat, res;
	CanMessage msg;
	
	stat = mcp2515_readStatus();
	
	
	if ( stat & MCP_STAT_RX0IF ) {
		// Msg in Buffer 0
		mcp2515_read_canMsg( MCP_RXBUF_0,&msg);
		mcp2515_modifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
		res = CAN_OK;
	}
	else if ( stat & MCP_STAT_RX1IF ) {
		// Msg in Buffer 1
		mcp2515_read_canMsg( MCP_RXBUF_1,&msg);
		mcp2515_modifyRegister(MCP_CANINTF, MCP_RX1IF, 0);
		res = CAN_OK;
	}
	else {
		res = CAN_NOMSG;
	}	             
//    CONSOL_Printf(msg.dta);
//    CONSOL_Printf(" ");  
  
}

/** CAN总线初始化函数 
*/
void CAN_Init(void)
{
    spi_init();
    mcp2515_Init(CAN_20KBPS);
    rGPGCON&=0xffffff3f;
    rGPGCON|=(1<<7);
    rGPGUP&=0xfff7;
    rGPGDAT|=(1<<3);
	rINTMSK &= ~(BIT_EINT8_23);
	rEINTMASK&=~(1<<11);
}


/** CAN总线中断响应函数 
*/
void __irq CAN_LISR(void)
{
    int temp;
    
    unsigned char stat, res;
	CanMessage msg;
	
	stat = mcp2515_readStatus();
	
	
	if ( stat & MCP_STAT_RX0IF ) {
		// Msg in Buffer 0
		mcp2515_read_canMsg( MCP_RXBUF_0,&msg);
		mcp2515_modifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
		res = CAN_OK;
	}
	else if ( stat & MCP_STAT_RX1IF ) {
		// Msg in Buffer 1
		mcp2515_read_canMsg( MCP_RXBUF_1,&msg);
		mcp2515_modifyRegister(MCP_CANINTF, MCP_RX1IF, 0);
		res = CAN_OK;
	}
	else {
		res = CAN_NOMSG;
		mcp2515_modifyRegister(MCP_CANINTF, 0xff, 0x00);
	    temp=rEINTPEND;
	    rEINTPEND=temp;
		temp = rSRCPND;
		rSRCPND = temp;
		temp = rINTPND;
		rINTPND = temp;
		
	    rGPGDAT|=(1<<3);
	    return;
	}	     
    strcpy(messagebuf,msg.dta);
    messagebuf[msg.dlc]='\0';
    
 //   CONSOL_Printf(messagebuf);
 //   PrintTextEdit("收到信息->$",RGB(255,255,0),65,150,280,40,1);
	PrintTextEdit(messagebuf,RGB(255,255,0),65,150,280,40,1);
/*	
	for (i=0; i<CAN_MAX_CHAR_IN_MESSAGE; i++)
	{
		if (msg.dta[i] == 0)
		{
			i = CAN_MAX_CHAR_IN_MESSAGE;
		}
		else
		{
			if (RxMsgBufW >= 256)
				RxMsgBufW = 0;
			
			RxMsgBuf[RxMsgBufW] = msg.dta[i];
			RxMsgBufW++;

		}
	}
*/
    
    temp=rEINTPEND;
    rEINTPEND=temp;
	temp = rSRCPND;
	rSRCPND = temp;
	temp = rINTPND;
	rINTPND = temp;
	
    rGPGDAT|=(1<<3);
 
}

int ReadForRxMsgBuf(char *msg)
{
	int i = 0;
	
	for(i=0; ;i++)
	{
	
		if (RxMsgBufR >= 256)
			RxMsgBufR = 0;
		
		if (RxMsgBufR < RxMsgBufW)
		{
			msg[i] = RxMsgBuf[RxMsgBufR];
			RxMsgBufR++;
		}
		else
		{
			if (i > 0)
				return TRUE;
			return FALSE;
		}		
	}
	return FALSE;
}