#include <stdarg.h>
#include <stdio.h>
#include "def.h"
#include "touch.h"
#include "UartLib.h"
#include "2410lib.h"
#include "2410addr.h"
#include "Int.h"

UART_PORT UartPort0;		// 串口0对应的结构体
int UartRxEndFlag;

/*
|	函数:	Receive_Uart0,	串口0接收中断程序
|	参数:	无
|	返回:	无
|
|	日期:	06/12/18
|	创建人:	ZengJC
*/
void __irq Receive_Uart0(void)
{
//	int temp;
	U8 receive;
	INT_ADC_Enable(FALSE);
	
	if (UartPort0.RxBufferStatus != BUFFER_FULL)
		do
		{
			receive = RxUart(UART0);
			UartPort0.RxBuffer[UartPort0.RxBufferWrite++] = receive;
		
			// 检查缓冲区
			if (UartPort0.RxBufferWrite == UartPort0.bufferSize)
				UartPort0.RxBufferWrite = 0;
			if (UartPort0.RxBufferWrite == UartPort0.RxBufferRead)
				UartPort0.RxBufferStatus = BUFFER_FULL;
			else
				UartPort0.RxBufferStatus = BUFFER_DATA;
			
		} while (rUTRSTAT0 & 0x1);
		
	
	ClearInt();			// 清中断
	INT_ADC_Enable(TRUE);
}

/*
|	函数:	UartGetChar,		从串口缓冲区提取一个字符
|	参数:	UART_PORT *port,	串口对应结构体
|	返回:	U8,					返回取得的字符,类型为U8(unsigned char)
|
|	日期:	06/12/18
|	创建人:	ZengJC
*/
U8 UartGetChar(UART_PORT *port)
{
	U8 ch = 0;
	
	if (port->RxBufferStatus != BUFFER_EMPTY) 
	{
		ch = port->RxBuffer[port->RxBufferRead++];
		
		if (port->RxBufferRead == port->bufferSize)
			port->RxBufferRead = 0;
		
		if (port->RxBufferRead == port->RxBufferWrite)
			port->RxBufferStatus = BUFFER_EMPTY;
		else
			port->RxBufferStatus = BUFFER_DATA;
	}
	
	Delay(2);
	return ch;
}

/*
|	函数:	SetDefaultToUart,	把串口结构体设为默认值
|	参数1:	UART_PORT *port,	串口对应结构体
|	参数2:	int com,			目标串口
|	返回:	无
|
|	日期:	06/12/18
|	创建人:	ZengJC
*/
void SetDefaultToUart(UART_PORT *port, int com)
{
	port->comPort	= com;		// 目标串口
	port->baudRate	= 112500;	// 波特率
	port->dataBits	= 3;		// 8 bits per frame
	port->stopBits	= 0;		// One stop bit per frame
	port->parity	= 0;		// No parity
	port->dataMode	= 0;		// UART runs in normal operation
	port->communicationMode = 0;// ???
	port->bufferSize = 1024;	// 接收缓冲大小
}

/*
|	函数:	InitUartPort,		初始化串口
|	参数:	UART_PORT *port,	串口对应结构体
|	返回:	int,				保留(未使用)
|
|	日期:	06/12/18
|	创建人:	ZengJC
*/
int InitUartPort(UART_PORT *port)
{
//	int temp;
	
	if (port->comPort == UART0)
	{
		port->vector = 28;
		port->baseAddr = UART0_BASE;
	}
	else
		if (port->comPort == UART1)
		{
			port->vector = 23;
			port->baseAddr = UART1_BASE;
		}
		else
			if (port->comPort == UART2)
			{
				port->vector = 15;
				port->baseAddr = UART2_BASE;
			}
			else
				return 0;
	
	// Setup the RX SD buffer
	port->RxBufferRead = port->RxBufferWrite = 0;
	port->RxBufferStatus = BUFFER_EMPTY;
	
	// Clear Control registers
	SET_REGISTER(port->baseAddr + rULCON_OFFSET, 0);
	SET_REGISTER(port->baseAddr + rUCON_OFFSET, 0);
	SET_REGISTER(port->baseAddr + rUFCON_OFFSET, 0);
	SET_REGISTER(port->baseAddr + rUMCON_OFFSET, 0);
	
	// Setup baud rate
	SetBaudrate(port->baudRate, port->comPort);
	
	// Setup Mode, Parity, Stop Bits and Data Size in ULCON Reg.
	SET_REGISTER(port->baseAddr + rULCON_OFFSET
				,port->parity | port->dataBits | port->stopBits);
	rUCON0  = 0x245;
	
	//do{
	//	temp = GET_REGISTER(port->baseAddr + rURXH_OFFSET);
	//} while (GET_REGISTER(port->baseAddr + rUTRSTAT_OFFSET) & 1);
	
	UartInterrupt(TRUE, port->comPort);
	
	return 0;
}

/*
|	函数:	SetBaudrate,	设置串口波特率
|	参数1:	U32 baudRate,	波特率
|	参数2:	int com,		目标串口
|	返回:	无
|
|	日期:	06/12/18
|	创建人:	ZengJC
*/
void SetBaudrate(U32 baudRate, int com)
{
	int pclk = 202800000/4;
	switch(com)
	{
	case UART0:
		{
			rUBRDIV0 = (int)(pclk/16./baudRate + 0.5) - 1;
		}break;
	case UART1:
		{
			rUBRDIV1 = (int)(pclk/16./baudRate + 0.5) - 1;
		}break;
	case UART2:
		{
			rUBRDIV2 = (int)(pclk/16./baudRate + 0.5) - 1;
		}break;
	default:
		{
		}break;
	}
}

/*
|	函数:	UartPutChar,		向串发送一个字符
|	参数1:	U8 ch,				将要发送的字符
|	参数2:	UART_PORT *port,	目标串口
|	返回:	无
|
|	日期:	06/12/18
|	创建人:	ZengJC
*/
void UartPutChar(U8 ch, UART_PORT *port)
{
	int status;
	status = GET_REGISTER(port->baseAddr + rUTRSTAT_OFFSET);

	while(!(status & 0x2));
	Delay(2);
	
	switch(port->comPort)
	{
	case UART0:
		{
			WrUTXH0(ch);
		}break;
	case UART1:
		{
			WrUTXH1(ch);
		}break;
	case UART2:
		{
			WrUTXH2(ch);
		}break;
	}
}

void UartSimplePutChar(U8 ch, int com)
{
	switch(com)
	{
	case UART0:
		{
			while(rUFSTAT0 & 0x2);	//Wait until THR is empty.
			Delay(2);
			WrUTXH0(ch);
		}break;

	case UART1:
		{
			while(rUFSTAT1 & 0x2);	//Wait until THR is empty.
			Delay(2);
			WrUTXH1(ch);
		}break;

	case UART2:
		{
			while(rUFSTAT2 & 0x2);	//Wait until THR is empty.
			Delay(2);
			WrUTXH2(ch);
		}break;
	}
}

/*
|	函数:	UartPutStr,			向串发送一个字符串
|	参数1:	char *str,			将要发送的字符
|	参数2:	UART_PORT *port,	目标串口
|	返回:	无
|
|	日期:	06/12/18
|	创建人:	ZengJC
*/
void UartPutStr(char *str, UART_PORT *port)
{
	while(*str)
		UartPutChar(*str++, port);
}

void UartSimplePutStr(char *str, int com)
{
	while(*str)
		UartSimplePutChar(*str++, com);
}

void UartPrintf(int com, char *strFmt,...)
{
	va_list ap;
	char String[256];
	
	va_start(ap, strFmt);
	vsprintf(String, strFmt, ap);
	UartSimplePutStr(String,com);
	va_end(ap);
}

/*
|	函数:	RxUart,		从串口接收一个字符
|	参数:	int com,	目标串口
|	返回:	U8			收到的字符
|
|	日期:	06/12/18
|	创建人:	ZengJC
*/
U8 RxUart(int com)
{
    switch(com)
    {
        case UART0:
                return (char)RdURXH0();
                break;
                
        case UART1:
                return (char)RdURXH1();
                break;
                
        case UART2:
                return (char)RdURXH2();
                break;
    }
    return 0;
}

/*
|	函数:	RxUartS,	从串口接收一个字符(带返回状态)
|	参数:	char *data,	收到的字符
|	参数:	int Uart,	目标串口
|	返回:	int			返回状态;TRUE,成功;FALSE失败
|
|	日期:	06/12/18
|	创建人:	ZengJC
*/
int RxUartS(char *data,int Uart)
{
	switch(Uart)
	{
		case UART0:
			if(rUFSTAT0 & 0x1)
			{
				*data = rURXH0;
				return TRUE;
			}break;
			
		case UART1:
			if(rUFSTAT1 & 0x1)
			{
				*data = rURXH1;
				return TRUE;
			}break;
			
		case UART2:
			if(rUFSTAT2 & 0x1)
			{
				*data = rURXH2;
				return TRUE;
			}break;
    }
    return FALSE;
}

void UartWaitForStr(char *str, int Uart)
{
	char *tempStr = str;
	char ch = 0;
	int flag = 1;
	
	UartRxEndFlag = FALSE;
	while(ch !='\r' && UartRxEndFlag == FALSE)
	{
		flag = RxUartS(&ch,Uart);
		if (flag == TRUE)
		{
			if (ch == '\b')
			{
				if ((int)tempStr < (int)str)
				{
					UartPrintf(Uart ,"\b \b");
					str--;
				}
			}
			else
			{
				*str = ch;
				str++;
				UartSimplePutChar(ch,Uart);
			}
		}
	}
	
	UartRxEndFlag = TRUE;
	str--;
	*str = '\0';
	UartPrintf(Uart ,"\n\n");
}

/*
|	函数:	UartInterrupt,	串口中断使能
|	参数1:	int enable,		使能
|	参数2:	int com,		目标串口
|	返回:	无
|
|	日期:	06/12/18
|	创建人:	ZengJC
*/
void UartInterrupt(int enable, int com)
{
	switch(com)
	{
	case UART0:
		{
			if (enable == TRUE)
			{
				rINTMSK &= ~BIT_UART0;
				//rINTMOD &= ~BIT_UART0;
				rINTSUBMSK &= ~(BIT_SUB_RXD0 | BIT_SUB_RXD0);
			}else{
				rINTMSK |= BIT_UART0;
				rINTSUBMSK |= BIT_SUB_RXD0;
			}
		}break;
	case UART1:
		{
			if (enable == TRUE)
			{
				rINTMSK &= ~BIT_UART1;
				rINTSUBMSK &= ~BIT_SUB_RXD1;
			}else{
				rINTMSK |= BIT_UART1;
				rINTSUBMSK |= BIT_SUB_RXD1;
			}
		}break;
	case UART2:
		{
			if (enable == TRUE)
			{
				rINTMSK &= ~BIT_UART2;
				rINTSUBMSK &= ~BIT_SUB_RXD2;
			}else{
				rINTMSK |= BIT_UART2;
				rINTSUBMSK |= BIT_SUB_RXD2;
			}
		}break;
	default:
		{
		}break;
	}
}

void UartDefaultInit(int Uart)
{
	switch(Uart)
	{
	case UART0:
		{
			rUFCON0	= 0;
			rUMCON0	= 0;
			rULCON0	= 0x23;
			rUCON0	= 0x245;
		}break;
	case UART1:
		{
			rUFCON1	= 0;
			rUMCON1	= 0;
			rULCON1	= 0x23;
			rUCON1	= 0x245;
		}break;
	case UART2:
		{
			rUFCON2	= 0;
			rUMCON2	= 0;
			rULCON2	= 0x23;
			rUCON2	= 0x245;
		}break;
	}
}