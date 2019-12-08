/*!	\file		spi.h
	\brief		CAN总线收发数据头文件
	\author		丁雷
	\version	0.1
*/
#ifndef _SPI_H_
#define _SPI_H_

#define ucSpiBaud 10000000



#define CAN_RESET       0xC0
#define CAN_READ        0x03
#define CAN_WRITE       0x02
#define CAN_RTS         0x80
#define CAN_RTS_TXB0    0x81
#define CAN_RTS_TXB1    0x82
#define CAN_RTS_TXB2    0x84
#define CAN_RD_STATUS   0xA0
#define CAN_BIT_MODIFY  0x05  
#define CAN_RX_STATUS   0xB0
#define CAN_RD_RX_BUFF  0x90
#define CAN_LOAD_TX     0X40  


/**SPI写函数
*  @param[in] DataOut   Single data byte for SPI bus.               
*/
void WriteSPI(unsigned char DataOut);

/**SPI读函数
*  @param[out]  Single data byte out from SPI bus.               
*/
unsigned char ReadSPI( void );
void SPIByteWrite(unsigned char addr, unsigned char value );
unsigned char SPIByteRead(unsigned char addr);
void SPIReset(void);
unsigned char spi_write(unsigned char data);
unsigned char spi_read(void);
void spi_init(void);

#endif
