/*!	\file		CAN.h
	\brief		CAN总线收发数据头文件
	\author		丁雷
	\version	0.1
*/
#ifndef	CAN_H
#define CAN_H


#define CAN_MAX_CHAR_IN_MESSAGE (8)	//!<MAX_CHAR_IN_MESSAGE

#define CAN_OK         (0)	//!<OK
#define CAN_FAILINIT   (1)	//!<FAILTX
#define CAN_FAILTX     (2)	//!<FAILTX
#define CAN_MSGAVAIL   (3)	//!<MSGAVAIL
#define CAN_NOMSG      (4)	//!<NOMSG
#define CAN_CTRLERROR  (5)	//!<CTRLERROR
#define CAN_FAIL       (0xff)	//!<FAIL

#define CAN_STDID (0)	//!<STDID
#define CAN_EXTID (1)	//!<EXTID

#define CANDEFAULTIDENT    (0x1111)	//!<DEFAUTIDENT
#define CANDEFAULTIDENTEXT (0x0)	//!<DEFAULTIDENTEXT

// 自定义命令
#define RED_LIGHT_OPEN			101
#define RED_LIGHT_CLOSE			102
#define GREEN_LIGHT_OPEN		103
#define GREEN_LIGHT_CLOSE		104
#define BLUE_LIGHT_OPEN			105
#define BLUE_LIGHT_CLOSE		106

extern char CAN_Message[8];



typedef struct {
	
	unsigned char  extended_identifier;  /*!< identifier CAN_xxxID */ 
	
	unsigned int identifier; /*!< either extended (the 29 LSB) or standard (the 11 LSB) */ 
	
	unsigned int  dlc;	//!< data length:
	char  dta[CAN_MAX_CHAR_IN_MESSAGE];
	
	//!<used for receive only:
	//!< Received Remote Transfer Bit 
	//!< (0=no... 1=remote transfer request received)
	unsigned char  rtr;  
	
	unsigned char  filhit;	//!<Acceptence Filter that enabled the reception
} CanMessage;



#define CAN_20KBPS   1				//!<CAN总线比特率20k
#define CAN_125KBPS  CAN_20KBPS+1	//!<CAN总线比特率125k

/** 数据发送函数 
*  @param[in] value 要发送的数据
*  @param[in] n		要发送的数据的长度
*/
void CAN_Write(char * value,int n);
/** CAN总线初始化函数 
*/
void CAN_Init(void); //CAN总线初始化
/** CAN总线中断响应函数 
*/
void __irq CAN_LISR(void); 

/** CAN总线发送数据函数 
*  @param[in] msg 待发送的CanMessage结构体
*  @param[out] 发送数据是否成功状态
*/
unsigned char can_sendMessage(CanMessage* msg);

int ReadForRxMsgBuf(char *msg);
void can_initBuf(void);

#endif


/*!	\mainpage	CAN总线实验
	\author		 丁雷
	\version 1.0
	\date    2006-9-1
	\section ABSTRACT 概要
        \n  在两台嵌入式实验设备之间实现主、从站工作模式。主站和从站之间可以信息数据的互相传感；主站对从站的远程控制；
        \n  从站实验设备状态的改变可以及时的反映到主站的相关设备上。
	\section HARDWARE 硬件
	\n   2410实验箱、CAN总线扩展卡(MCP2515,MCP2551)
	\section SOFTERWARE 软件
	\n   DEMO实验平台
*/	
/*!	\defgroup	Introduction   使用说明
        \n每一块S3C2410实验板都可以选择主站/从站模式（通过触摸屏上自绘按钮的选择）。当两块实验板的主/从站模式选择好了以后，两块实验板上CPU，CAN总线控制器，CAN总线收发器等设备的初始化就会接着完成。两个实验板分别进入主站或从站窗口。
	\image html images\main.gif
	\n主站窗口
	\n"＋"：点击一下，将"数值显示区"中的原有数值加1并在从站实验板的"数值显示区"显示。
	\n"－"：点击一下，将"数值显示区"中的原有数值减1并在从站实验板的"数值显示区"显示。
	\n红灯开按钮：点亮从站实验板上的红灯，并且在从站窗口中红灯旁边显示"开"。
	\n红灯关按钮：熄灭从站实验板上的红灯，并且在从站窗口中红灯旁边显示"关"。
	\n绿灯开按钮：点亮从站实验板上的绿灯，并且在从站窗口中绿灯旁边显示"开"。
	\n绿灯关按钮：熄灭从站实验板上的绿灯，并且在从站窗口中绿灯旁边显示"关"。
	\n多行文本框：用于显示从站实验板上的一些操作记录，例如：外部中断键按下等。

	\image html images\master.gif
	\n从站窗口
	\n"＋"：点击一下，将"数值显示区"中的原有数值加1并在主站实验板的"数值显示区"显示。
	\n"－"：点击一下，将"数值显示区"中的原有数值减1并在主站实验板的"数值显示区"显示。
	\n多行文本框：用于显示从站本身实验板上的一些操作记录，例如：外部中断按下等。
	
	\image html images\slave.gif

*/
/*!	\defgroup CONSTRUCTION   《CAN总线》软件模块结构图
	\n	CAN总线实验分为3大模块：应用层模块，SPI读写模块和MCP2515驱动程序。这三个模块之间的调用关系如图所示：	
	\image html images\construction.gif

*/
/*!	\defgroup SPI   SPI读写模块
	\n	SPI读函数：unsigned char ReadSPI( void )
        \n           返回：从SPI0接口读取一个字节的数据
	\n	ReadSPI程序流程图如下：
	\image html images\spiread.gif
	\n	SPI写函数：void WriteSPI(unsigned char DataOut)
        \n           输入：要通过SPI0接口输出的一字节数据
	\n	WriteSPI程序流程图如下：
	\image html images\spiwrite.gif
*/
/*!	\defgroup MCP2515   MCP2515驱动程序
	\n	MCP2515芯片初始化函数：unsigned char mcp2515_Init(const unsigned char canSpeed)
        \n      输入：MCP2515的位速率
        \n	输出：MCP2515初始化成功与否的状态
	\n	mcp2515_Init程序流程图如下：
	\image html images\mcp2515init.gif
	
	\n	MCP2515申请发送缓冲器函数：unsigned char mcp2515_getNextFreeTXBuf(unsigned char *txbuf_n)
        \n      输入：一个字符，保存申请到的发送缓冲器的序号
	\n	输出：MCP2515申请发送缓冲器成功与否的状态
	\n	mcp2515_getNextFreeTXBuf程序流程图如下：
	\image html images\mcp2515_getNextFreeTXBuf.gif
	
	\nMCP2515写发送缓冲器函数：void mcp2515_write_canMsg(unsigned char buffer_sidh_addr, CanMessage * msg)
	\n输入：buffer_sidh_addr--发送缓冲器地址    Msg--要在CAN总线上发送的消息的结构体
	\n功能：将要发送的数据的内容，数据的长度，一帧数据的标识符等分别放入发送缓冲器n的相应配套寄存器中。
	
	\n
	\n  MCP2515启动一帧数据发送函数：void mcp2515_start_transmit(const unsigned char buffer_sidh_addr)
	\n输入：buffer_sidh_addr--发送缓冲器地址
	\n功能：根据发送缓冲器的地址，发送相应的SPI  RTS命令。即启动先前放入缓冲器中的一帧数据的发送。
	\n 缓冲器		RTS命令
	\n发送缓冲器0		0x81
	\n发送缓冲器1		0x82
	\n发送缓冲器2		0x84
	
	\n	MCP2515读状态寄存器函数： unsigned char mcp2515_readStatus(void)
        \n返回：状态寄存器中的数值
	\n	mcp2515_readStatus程序流程图如下：
	\image html images\mcp2515_readStatus.gif
	
	\n	MCP2515读一帧数据函数：void mcp2515_read_canMsg(unsigned char buffer_sidh_addr,CanMessage * msg)
	\n输入：buffer_sidh_addr--接收缓冲器地址    Msg--存放接收到的一帧数据的消息结构体
	\n功能：根据接收缓冲器地址buffer_sidh_addr，从接收缓冲器RXBn的相关寄存器中读取需要的一帧数据内容到消息结构体msg中。
	\n
	\n	MCP2515寄存器位修改函数：void mcp2515_modifyRegister(const unsigned char address, const unsigned char mask, const unsigned char data)
	\n输入：address--要修改的寄存器的地址
	\n      Mask--寄存器中要修改的相应位的掩码。（见参考资料2）
	\n      Data--需要修改的数据
	\image html images\mcp2515_modifyRegister.gif	

*/
/*!	\defgroup APPLICATION   应用层模块
	\n	CAN总线消息结构体：
	\n	typedef struct {
	\n		unsigned char  extended_identifier; //扩展标识符
	\n		unsigned int identifier; //标识符
	\n		unsigned int  dlc;	//发送数据长度
	\n		char  dta[CAN_MAX_CHAR_IN_MESSAGE];//待发送数据区
	\n		unsigned char  rtr;  //是否为远程发送请求
	\n		unsigned char  filhit;	//Acceptence Filter that enabled the reception
	\n	} CanMessage;

 	\n	CAN总线发送数据函数：unsigned char can_sendMessage(CanMessage* msg)
        \n      输入：msg--待发送的CanMessage结构体
        \n	输出：发送数据是否成功状态
	\n	WriteSPI程序流程图如下：
	\image html images\can_sendMessage.gif
	
	\n	数据发送函数：void CAN_Write(char * value, int n)
        \n      输入：value--发送的数据    n--发送的数据长度
        \n	功能：根据输入参数value，n初始化一个新的CanMessage结构体，然后调用can_sendMessage(CanMessage* msg)函数发送一帧数据。

 	\n	CAN总线中断服务程序：void __irq CAN_LISR(void)
	\n	CAN_LISR程序流程图如下：
	\image html images\CAN_LISR.gif

	
*/
