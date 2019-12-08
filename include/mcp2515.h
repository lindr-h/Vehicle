/*!	\file		mcp2515.h
	\brief		mcp2515芯片操作头文件
	\author		丁雷
	\version	0.1
*/

#ifndef MCP2515_H_
#define MCP2515_H_


#include "spi.h"
#include "2410addr.h"
#include "can.h"



#define MCP2515_OK         (0)	//!<操作成功
#define MCP2515_FAIL       (1)	//!<操作失败
#define MCP_ALLTXBUSY      (2)	//!<操作忙

#define MCP_N_TXBUFFERS    3	//!<MCP2515的发送缓冲器数量

#define MCP_RXBUF_0 (MCP_RXB0SIDH) //!<MCP2515的发送缓冲器0编号
#define MCP_RXBUF_1 (MCP_RXB1SIDH) //!<MCP2515的发送缓冲器1编号	



#define MCP_SIDH        0
#define MCP_SIDL        1
#define MCP_EID8        2
#define MCP_EID0        3

#define MCP_TXB_EXIDE_M     0x08    
#define MCP_DLC_MASK        0x0F    
#define MCP_RTR_MASK        0x40   

#define MCP_RXB_RX_ANY      0x60
#define MCP_RXB_RX_EXT      0x40
#define MCP_RXB_RX_STD      0x20
#define MCP_RXB_RX_STDEXT   0x00
#define MCP_RXB_RX_MASK     0x60
#define MCP_RXB_BUKT_MASK   0x04


#define MCP_TXB_TXBUFE_M    0x80
#define MCP_TXB_ABTF_M      0x40
#define MCP_TXB_MLOA_M      0x20
#define MCP_TXB_TXERR_M     0x10
#define MCP_TXB_TXREQ_M     0x08
#define MCP_TXB_TXIE_M      0x04
#define MCP_TXB_TXP10_M     0x03

#define MCP_TXB_RTR_M       0x40    
#define MCP_RXB_IDE_M       0x08    
#define MCP_RXB_RTR_M       0x40    

#define MCP_STAT_RXIF_MASK   (0x03)
#define MCP_STAT_RX0IF (1<<0)
#define MCP_STAT_RX1IF (1<<1)

#define MCP_EFLG_RX1OVR (1<<7)
#define MCP_EFLG_RX0OVR (1<<6)
#define MCP_EFLG_TXBO   (1<<5)
#define MCP_EFLG_TXEP   (1<<4)
#define MCP_EFLG_RXEP   (1<<3)
#define MCP_EFLG_TXWAR  (1<<2)
#define MCP_EFLG_RXWAR  (1<<1)
#define MCP_EFLG_EWARN  (1<<0)
#define MCP_EFLG_ERRORMASK  (0xF8) 



#define MCP_RXF0SIDH	0x00	//!<RXF0SIDH
#define MCP_RXF0SIDL	0x01	//!<RXF0SIDL
#define MCP_RXF0EID8	0x02	//!<RXF0EID8
#define MCP_RXF0EID0	0x03	//!<RXF0EID0
#define MCP_RXF1SIDH	0x04	//!<RXF1SIDH
#define MCP_RXF1SIDL	0x05	//!<RXF1SIDL
#define MCP_RXF1EID8	0x06	//!<RXF1EID8
#define MCP_RXF1EID0	0x07	//!<RXF1EID0
#define MCP_RXF2SIDH	0x08	//!<RXF2SIDH
#define MCP_RXF2SIDL	0x09	//!<RXF2SIDL
#define MCP_RXF2EID8	0x0A	//!<RXF2EID8
#define MCP_RXF2EID0	0x0B	//!<RXF2EID0
#define MCP_CANSTAT		0x0E	//!<CANSTAT	
#define MCP_CANCTRL		0x0F	//!<CANCTRL
#define MCP_RXF3SIDH	0x10	//!<RXF3SIDH
#define MCP_RXF3SIDL	0x11	//!<RXF3SIDL
#define MCP_RXF3EID8	0x12	//!<RXF3EID8
#define MCP_RXF3EID0	0x13	//!<RXF3EID0
#define MCP_RXF4SIDH	0x14	//!<RXF4SIDH
#define MCP_RXF4SIDL	0x15	//!<RXF4SIDL
#define MCP_RXF4EID8	0x16	//!<RXF4EID8
#define MCP_RXF4EID0	0x17	//!<RXF4EID0
#define MCP_RXF5SIDH	0x18	//!<RXF5SIDH
#define MCP_RXF5SIDL	0x19	//!<RXF5SIDL
#define MCP_RXF5EID8	0x1A	//!<RXF5EID8
#define MCP_RXF5EID0	0x1B	//!<RXF5EID0
#define MCP_TEC			0x1C	//!<TEC
#define MCP_REC			0x1D	//!<REC	
#define MCP_RXM0SIDH	0x20	//!<RXM0SIDH
#define MCP_RXM0SIDL	0x21	//!<RXM0SIDL
#define MCP_RXM0EID8	0x22	//!<RXM0EID8
#define MCP_RXM0EID0	0x23	//!<RXM0EID0
#define MCP_RXM1SIDH	0x24	//!<RXM1SIDH
#define MCP_RXM1SIDL	0x25	//!<RXM1SIDL
#define MCP_RXM1EID8	0x26	//!<RXM1EID8
#define MCP_RXM1EID0	0x27	//!<RXM1EID0
#define MCP_CNF3		0x28	//!<CNF3
#define MCP_CNF2		0x29	//!<CNF2
#define MCP_CNF1		0x2A	//!<CNF1
#define MCP_CANINTE		0x2B	//!<CANINTE
#define MCP_CANINTF		0x2C	//!<CANINTF
#define MCP_EFLG		0x2D	//!<EFLG
#define MCP_TXB0CTRL	0x30	//!<TXB0CTRL
#define MCP_TXB1CTRL	0x40	//!<TXB1CTRL
#define MCP_TXB2CTRL	0x50	//!<TXB2CTRL
#define MCP_RXB0CTRL	0x60	//!<RXB0CTRL
#define MCP_RXB0SIDH	0x61	//!<RXB0SIDH
#define MCP_RXB1CTRL	0x70	//!<RXB1CTRL
#define MCP_RXB1SIDH	0x71	//!<RXB1SIDH


#define MCP_TX_INT		0x1C		// Enable all transmit interrupts
#define MCP_TX01_INT	0x0C		// Enable TXB0 and TXB1 interrupts
#define MCP_RX_INT		0x03		// Enable receive interrupts
#define MCP_NO_INT		0x00		// Disable all interrupts

#define MCP_TX01_MASK	0x14
#define MCP_TX_MASK		0x54

#define MCP_WRITE		0x02	//!<WRITE

#define MCP_READ		0x03	//!<READ

#define MCP_BITMOD		0x05	//!<BITMOD

#define MCP_LOAD_TX0	0x40	//!<LOAD_TX0
#define MCP_LOAD_TX1	0x42	//!<LOAD_TX1
#define MCP_LOAD_TX2	0x44	//!<LOAD_TX2

#define MCP_RTS_TX0		0x81	//!<RTS_TX0
#define MCP_RTS_TX1		0x82	//!<RTS_TX1
#define MCP_RTS_TX2		0x84	//!<RTS_TX2	
#define MCP_RTS_ALL		0x87	//!<RTS_ALL

#define MCP_READ_RX0	0x90	//!<READ_RX0
#define MCP_READ_RX1	0x94	//!<READ_RX1
	
#define MCP_READ_STATUS	0xA0	//!<READ_STATUS	

#define MCP_RX_STATUS	0xB0	//!<RX_STATUS

#define MCP_RESET		0xC0	//!<RESET

#define MODE_NORMAL     0x00	//!<正常模式
#define MODE_SLEEP      0x20	//!<睡眠模式
#define MODE_LOOPBACK   0x40	//!<环回路模式
#define MODE_LISTENONLY 0x60	//!<监听模式
#define MODE_CONFIG     0x80	//!<配置模式
#define MODE_POWERUP	0xE0	//!<上电模式
#define MODE_MASK		0xE0	
#define ABORT_TX        0x10	
#define MODE_ONESHOT	0x08	
#define CLKOUT_ENABLE	0x04	
#define CLKOUT_DISABLE	0x00	
#define CLKOUT_PS1		0x00	
#define CLKOUT_PS2		0x01	
#define CLKOUT_PS4		0x02	
#define CLKOUT_PS8		0x03	

// CNF1 Register Values

#define SJW1            0x00
#define SJW2            0x40
#define SJW3            0x80
#define SJW4            0xC0


// CNF2 Register Values

#define BTLMODE			0x80
#define SAMPLE_1X       0x00
#define SAMPLE_3X       0x40


// CNF3 Register Values

#define SOF_ENABLE		0x80
#define SOF_DISABLE		0x00
#define WAKFIL_ENABLE	0x40
#define WAKFIL_DISABLE	0x00


// CANINTF Register Bits

#define MCP_RX0IF		0x01
#define MCP_RX1IF		0x02
#define MCP_TX0IF		0x04
#define MCP_TX1IF		0x08
#define MCP_TX2IF		0x10
#define MCP_ERRIF		0x20
#define MCP_WAKIF		0x40
#define MCP_MERRF		0x80


#define MCP2515_SELECT()   (rGPGDAT &=~(0x04))	//!<MCP2515使能	
#define MCP2515_UNSELECT() (rGPGDAT |=(0x04))	//!<MCP2515取消选择	


/*-----------------------*/
/* global settings       */
/*-----------------------*/

/* CFG3 */

// Start of Frame SOF (CLKOUT)
// either SOF_ENABLE or SOF_DISABLE
#define MCP_GENERAL_SOF (SOF_DISABLE)

// Wake up Filter (WAKFIL)
// either WAKFIL_ENABLE	or WAKFIL_DISABLE
#define MCP_GENERAL_WAKFIL (WAKFIL_DISABLE)


/*-----------------------*/
/* 125 kBPS @ 4MHZ F_OSC */
/*-----------------------*/

/* CNF1 */

// SJW as defined in mcp_defs.h
// here: SJW = 1*TQ
#define MCP_4MHz_125kBPS_SJW (SJW1)

// Prescaler = (BRP+1)*2
// here Prescaler = 4 -> BRP=1
#define MCP_4MHz_125kBPS_BRP (1)


/* CNF2 */

// BLT-Mode defined in CNF3 (0 or BTLMODE from mcp_defs.h)
#define MCP_4MHz_125kBPS_BLTMODE (BTLMODE)

// 3 samples (SAMPLE_3X) or 1 sample (SAMPLE_1X)
#define MCP_4MHz_125kBPS_SAM   (SAMPLE_1X)

// (Phase Segment 1) PHSEG1 = PS1 - 1
// here: PS1 calculated as 2 
#define MCP_4MHz_125kBPS_PHSEG1 ((2-1)<<3)

// (Propagation Delay) PRSEG = PRSEQTQ-1
// here PRSEQTQ = 2
#define MCP_4MHz_125kBPS_PRSEG (2-1)

/* CNF3 */

// (Phase Segment 2) PHSEG2 = PS2 - 1
// here: PS2 calculated as 3
#define MCP_4MHz_125kBPS_PHSEG (3-1)


#define MCP_4MHz_125kBPS_CFG1 (MCP_4MHz_125kBPS_SJW | MCP_4MHz_125kBPS_BRP)
#define MCP_4MHz_125kBPS_CFG2 (MCP_4MHz_125kBPS_BLTMODE | MCP_4MHz_125kBPS_SAM | \
				MCP_4MHz_125kBPS_PHSEG1 | MCP_4MHz_125kBPS_PRSEG)
#define MCP_4MHz_125kBPS_CFG3 (MCP_GENERAL_SOF | MCP_GENERAL_WAKFIL | \
				MCP_4MHz_125kBPS_PHSEG)


/*-----------------------*/
/* 20 kBPS @ 4MHZ F_OSC  */
/*-----------------------*/

/* CNF1 */

// SJW as defined in mcp_defs.h
// here: SJW = 1*TQ
#define MCP_4MHz_20kBPS_SJW (SJW1)

// Prescaler = (BRP+1)*2
// here Prescaler = 10 -> BRP=4  (20TQ)
#define MCP_4MHz_20kBPS_BRP (4)


/* CNF2 */

// BLT-Mode defined in CNF3 (0 or BTLMODE from mcp_defs.h)
#define MCP_4MHz_20kBPS_BLTMODE (BTLMODE)

// 3 samples (SAMPLE_1X) or 1 sample (SAMPLE_1X)
#define MCP_4MHz_20kBPS_SAM   (SAMPLE_1X)

// (Phase Segment 1) PHSEG1 = PS1 - 1
// here: PS1 calculated as 8
#define MCP_4MHz_20kBPS_PHSEG1 ((8-1)<<3)

// (Propagation Delay) PRSEG = PRSEQTQ-1
// here PRSEQTQ = 3
#define MCP_4MHz_20kBPS_PRSEG (3-1)

/* CNF3 */

// (Phase Segment 2) PHSEG2 = PS2 - 1
// here: PS2 calculated as 8
#define MCP_4MHz_20kBPS_PHSEG (8-1)


#define MCP_4MHz_20kBPS_CFG1 (MCP_4MHz_20kBPS_SJW | MCP_4MHz_20kBPS_BRP)
#define MCP_4MHz_20kBPS_CFG2 (MCP_4MHz_20kBPS_BLTMODE | MCP_4MHz_20kBPS_SAM | \
				MCP_4MHz_20kBPS_PHSEG1 | MCP_4MHz_20kBPS_PRSEG)
#define MCP_4MHz_20kBPS_CFG3 (MCP_GENERAL_SOF | MCP_GENERAL_WAKFIL | \
				MCP_4MHz_20kBPS_PHSEG)

/** MCP2515RESET函数 
*/
void mcp2515_reset(void);

/** MCP2515读寄存器函数
*  @param[in] address 寄存器地址
*/
unsigned char mcp2515_readRegister(const unsigned char address);

/** MCP2515设置寄存器函数
*  @param[in] address 寄存器地址
*  @param[in] value 要写入到寄存器中的数值
*/
void mcp2515_setRegister(const unsigned char address, 
	const unsigned char value);
	
/** MCP2515改变寄存器中某几位函数
*  @param[in] address 寄存器地址
*  @param[in] mask    寄存器须修改位的掩码
*  @param[in] value 要写入到寄存器中的数值
*/
void mcp2515_modifyRegister(const unsigned char address, const unsigned char mask, 
	const unsigned char data);

/** MCP2515读状态寄存器函数
*  @param[out] 状态寄存器中的数据
*/
unsigned char mcp2515_readStatus(void);

/** MCP2515读发送状态寄存器函数
*  @param[out] 发送状态寄存器中的数据
*/
unsigned char mcp2515_RXStatus(void);

/** MCP2515设置CANCTRL寄存器函数
*  @param[in] newmode 要写入到寄存器中的新值
*/
unsigned char mcp2515_setCANCTRL_Mode(const unsigned char newmode);

/** MCP2515设置位速率函数
*  @param[in] canSpeed 位速率
*/
unsigned char mcp2515_configRate(const unsigned char canSpeed);

/** MCP2515芯片初始化函数
*  @param[in] canSpeed 位速率
*/
unsigned char mcp2515_Init(const unsigned char canSpeed);

/** MCP2515芯片写标识符函数
*  @param[in] mcp_addr 标识符寄存器地址
*  @param[in] ext 	   扩展标识符标志
*  @param[in] can_id   标识符值
*/
void mcp2515_write_can_id( const unsigned char mcp_addr, 
	const unsigned char ext, const unsigned int can_id );

void mcp2515_setRegisterS(unsigned char address,char values[],unsigned char n);

/** MCP2515芯片启动发送数据函数
*  @param[in] mcp_addr 发送寄存器地址
*/
void mcp2515_start_transmit(const unsigned char mcp_addr);


/** MCP2515读一帧数据函数
*  @param[in] buffer_sidh_addr 接收缓冲器地址
*  @param[in] Msg 存放接收到的一帧数据的消息结构体
*/
void mcp2515_read_canMsg(unsigned char buffer_sidh_addr,CanMessage * msg);

/** MCP2515申请发送缓冲器函数
*  @param[in] txbuf_n 一个字符，保存申请到的发送缓冲器的序号
*  @param[out] MCP2515申请发送缓冲器成功与否的状态
*/
unsigned char mcp2515_getNextFreeTXBuf(unsigned char *txbuf_n); 

/** MCP2515写发送缓冲器函数
*  @param[in] buffer_sidh_addr 发送缓冲器地址
*  @param[in] Msg 要在CAN总线上发送的消息的结构体
*/
void mcp2515_write_canMsg(unsigned char buffer_sidh_addr, CanMessage * msg);


void mcp2515_read_can_id(unsigned char mcp_addr,unsigned char * ext,unsigned int * can_id );

void mcp2515_readRegisterS(unsigned char address,char values[], unsigned char n);

#endif
