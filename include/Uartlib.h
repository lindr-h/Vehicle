#ifndef __UARTLIB_H__
#define __UARTLIB_H__

/* The base addresses for the seperate UART registers. */
#define UART0_BASE			0x50000000
#define UART1_BASE			0x50004000
#define UART2_BASE			0x50008000

#define UART0				1
#define UART1				2
#define UART2				3

#define BUFFER_FULL			1
#define BUFFER_DATA			2
#define BUFFER_EMPTY		3

#define WrUTXH(reg,ch)				*((U32*)(reg)) = (U8)ch
#define SET_REGISTER(reg, data)		*((U32*)(reg)) = (U32)data
#define GET_REGISTER(reg)			*((U32*)(reg))//(*(volatile unsigned *)0x50000010)

/* UART register offsets from the UART base */
#define rULCON_OFFSET		0x00        /* UART Line Control */
#define rUCON_OFFSET		0x04        /* UART Channel Control Register */
#define rUFCON_OFFSET		0x08        /* UART FIFO Control Register */
#define rUMCON_OFFSET		0x0C        /* UART Modem Control Register */
#define rUTRSTAT_OFFSET		0x10        /* UART Tx/Rx Status Register */
#define rUERSTAT_OFFSET		0x14        /* UART Rx error Status Register */
#define rUFSTAT_OFFSET		0x18        /* UART FIFO Status Register */
#define rUMSTAT_OFFSET		0x1C        /* UART Modem Status Register */
#define rUTXH_OFFSET		0x20        /* UART Trasmit Buffer Register */
#define rURXH_OFFSET		0x24        /* UART Receive Buffer Register */
#define rUBRDIV_OFFSET		0x28        /* UART Baud Rate Divisor Register */

typedef struct
{
	U32 dataMode;
	U32 baseAddr;
	
	U32 comPort;
	U32 dataBits;
	U32 stopBits;
	U32 parity;
	U32 baudRate;
	
	U32 vector;
	
	U32 driverOptions;
	U32 bufferSize;
	
	U32 parityErr;
	U32 frameErr;
	U32 overrunErr;
	U32 busyErr;
	U32 generalErr;
	
	U8  RxBuffer[1024];
	S32 RxBufferRead;
	S32 RxBufferWrite;
	S32 RxBufferStatus;
	
	U32 communicationMode;
}UART_PORT;

void __irq Receive_Uart0(void);
void SetDefaultToUart(UART_PORT *port, int com);
void SetBaudrate(U32 baudRate, int com);
int InitUartPort(UART_PORT *port);
void UartPutChar(U8 ch, UART_PORT *port);
void UartPutStr(char *str, UART_PORT *port);
void UartInterrupt(int enable, int com);
U8 RxUart(int UARTn);
U8 UartGetChar(UART_PORT *port);
void UartDefaultInit(int Uart);
int RxUartS(char *data, int Uart);
void UartSimplePutStr(char *str, int com);
void UartSimplePutChar(U8 ch, int com);
void UartPrintf(int com, char *strFmt,...);
void UartWaitForStr(char *str, int Uart);

#endif