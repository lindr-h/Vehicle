/*
This file mainly define the structs of command parameters and return parameters 
we often use and some operation for these structs.
*/ 
/*
**********************************************************************************************************
*                                               
*		    		       		hci DRIVERS FOR NUCLEUS                                       
*                                                                                
*                              (c) Copyright 2003-2015, cn         
*                                All Rights Reserved                 
*                                                                                
*                               hci DRIVERS Program for 1.0            
*
* File  Name 	: hci.h
* By    Write	: chenna
* First Write	: 2004-08-25 13:36
* Last  Write	: 2004-08-25 13:36
* where Write	: Room B201 Shenzhen International Tech-innovation Academy, Kejinan 10 Road,
*		   			High-Tech Industrial Park,Shenzhen,China.
* Function	: packet the hci command and analyze the event packet
*				1. 	
*				2. 				
*				3. 
***********************************************************************************************************
*/

#ifndef __HCI_H
#define __HCI_H


#include "sd_extr.h"

/*
***********************************************************************************************************
* 												基本数据定义（原ption.h）
*
***********************************************************************************************************
*/
#ifndef __DATA_TYPE_DEF_H__

#define VOID void
typedef signed 		char			INT8; 
//#define UINT8 						char
//#define INT16 						short   		 
//#define UINT16						short
//typedef signed 		int    			INT32; 
#define INT32	int
#define NULL			0

#endif



#define 		HCI_COMMAND_PKT				0x01
#define			HCI_ACLDATA_PKT 			0x02
#define			HCI_SCODATA_PKT 			0x03
#define			HCI_EVENT_PKT				0x04
#define			HCI_UNKNOWN_PKT				0xff


/* This section define the status of operation.It is used to debug.
//hdev->flags
#define HCI_INIT	0x0010
#define HCI_UP 		0x0020
#define HCI_RUNNING	0x0040
*/


#define HCI_PSCAN	0x0100	//page scan flags
#define HCI_ISCAN	0x0200  //inquiry scan flags
#define HCI_AUTH	0x0400  //authen flags

//link type
#define SCO_LINK	0x00
#define ACL_LINK	0x01

//packet type
#define HCI_DM1 	0x0008
#define HCI_DM3 	0x0400
#define HCI_DM5 	0x4000
#define HCI_DH1 	0x0010
#define HCI_DH3 	0x0800
#define HCI_DH5 	0x8000

//time slot
#define LMP_3SLOT	0x01
#define LMP_5SLOT	0x02
#define LMP_ENCRYPT	0x04
#define LMP_SOFFSET	0x08
#define LMP_TACCURACY	0x10
#define LMP_RSWITCH	0x20
#define LMP_HOLD	0x40
#define LMP_SNIF	0x80
  
#define LMP_PARK	0x01
#define LMP_RSSI	0x02
#define LMP_QUALITY	0x04
#define LMP_SCO		0x08
#define LMP_HV2		0x10
#define LMP_HV3		0x20
#define LMP_ULAW	0x40
#define LMP_ALAW	0x80
    
#define LMP_CVSD	0x01
#define LMP_PSCHEME	0x02
#define LMP_PCONTROL	0x04


					/*******************************************************/
					/***************	HCI Commands 命令定义	************/
					/*********************OGF & OCF values******************/
/*
***********************************************************************************************************
*											Link Control
* 
***********************************************************************************************************
*/

#define 		OGF_LINK_CTL				0x01

#define 		OCF_INQUIRY					0x0001
typedef __packed struct{
	char lap[3];
	char length;
	char num_rsp;
}inquiry_cp;
#define 		INQUIRY_CP_SIZE 			5
	#define DEFAULT_INQUIRY_LENGTH 0xa
#define			INQUIRY_CANCEL				0x0002
#define 		OCF_CREATE_CONN				0x0005
typedef __packed struct {
	UINT8	    bdaddr[6];
	UINT16 		pkt_type;
	UINT8 		pscan_rep_mode;
	UINT8 		pscan_mode;
	UINT16 		clock_offset;
	UINT8 		role_switch;
}create_conn_cp;
#define 		CREATE_CONN_CP_SIZE 		13
#define 		OCF_DISCONNECT				0x0006
typedef __packed struct {
	UINT16 		handle;
	UINT8 		reason;
}disconnect_cp;
#define 		DISCONNECT_CP_SIZE 			3

#define			OCF_ADD_SCO_CONN			0x0007
typedef __packed struct{
	UINT16		handle;
	UINT16		pkt_type;
}add_sco_con_cp;
#define 		ADD_SCO_CONN_SIZE			4
#define 		OCF_ACCEPT_CONN_REQ			0x0009
typedef __packed struct {
	UINT8		bdaddr[6];
	UINT8 		role;
}accept_conn_req_cp;
#define 		ACCEPT_CONN_REQ_CP_SIZE		7
#define			OCF_REJECT_CONN_REQ			0x000A
typedef __packed struct{
	UINT8 		bdaddr[6];
	UINT8		reason;
}reject_conn_cp;
#define 		REJECT_CONN_REQ_CP_SIZE		7
#define			OCF_REMOTE_NAME_REQ			0x0019
typedef __packed struct{
	UINT8 bdaddr[6];
	UINT8 		pscan_rep_mode;
	UINT8 		pscan_mode;
	UINT8 		clock_offset;
}remote_name_req_cp;
#define			REMOTE_NAME_REQ_CP_SIZE			10
#define			OCF_READ_CLK_OFFSET			0x001F

/*
***********************************************************************************************************
*											Link Policy
* 
***********************************************************************************************************
*/

#define OGF_LINK_POLICY	 					0x02

/*
***********************************************************************************************************
*											Host Controller and Baseband
* 
***********************************************************************************************************
*/

#define 		OGF_HOST_CTL				0x03

#define			OCF_EVT_MSK					0x0001

#define 		OCF_RESET					0x0003
#define 		OCF_SET_EVENT_FLT			0x0005
typedef __packed struct{
	UINT8		flt_type;
	UINT8		cond_type;
	UINT8		condition;
}set_event_flt_cp;

	/* Filter types */
	#define FLT_CLEAR_ALL		0x00
	#define FLT_INQ_RESULT		0x01
	#define FLT_CONN_SETUP		0x02
	/* CONN_SETUP Condition types */
	#define CONN_SETUP_ALLOW_ALL	0x00
	#define CONN_SETUP_ALLOW_CLASS	0x01
	#define CONN_SETUP_ALLOW_BDADDR	0x02
	 /* CONN_SETUP Conditions */
	#define CONN_SETUP_AUTO_OFF	0x01
	#define CONN_SETUP_AUTO_ON	0x02
#define			OCF_FLUSH					0x0008
#define			OCF_CHANGE_LOCAL_NAME	0x0013
#define 		OCF_READ_LOCAL_NAME			0x0014
typedef __packed struct {
	UINT8		status;
	UINT8	 	name[248];
}read_local_name_rp;
#define READ_LOCAL_NAME_RP_SIZE 	249 
#define 		OCF_WRITE_CA_TIMEOUT  		0x0016
#define 		OCF_WRITE_PG_TIMEOUT		0x0018
#define 		OCF_WRITE_SCAN_ENABLE 		0x001A
	#define SCANS_DISABLED		0x00
	#define IS_ENA_PS_DIS		0x01
	#define IS_DIS_PS_ENA		0x02
	#define IS_ENA_PS_ENA		0x03

#define			OCF_READ_PAGE_SCAN_ACTIVITY		0x001B
typedef __packed struct{
	UINT8		status;
	UINT16		page_scan_interval;
	UINT16		page_scan_window;
}read_page_scan_act_rp;

#define			OCF_WRITE_PAGE_SCAN_ACTIVITY	0x001C
typedef __packed struct{
	UINT16		page_scan_interval;
	UINT16		page_scan_window;
}write_page_scan_act_cp;
#define 		OCF_WRITE_AUTH_ENABLE		0x0020
	#define AUTH_DISABLED		0x00
	#define AUTH_ENABLED		0x01
#define 		OCF_READ_CLASS_OF_DEV		0x0023
typedef __packed struct {
	UINT8	status;
	UINT8 	dev_class[3];
}read_class_of_dev_rp;
#define 		READ_CLASS_OF_DEV_RP_SIZE 	4
#define 		OCF_WRITE_CLASS_OF_DEV		0x0024
typedef __packed struct {
	UINT8 	dev_class[3];
} write_class_of_dev_cp;
#define 		WRITE_CLASS_OF_DEV_CP_SIZE 	3

#define			OCF_WRITE_VOICE_SETTING		0x0026
	#define IN_CODING_LINEAR		0<<8
	#define IN_CODING_U				1<<8
	#define IN_CODING_A				2<<8
	#define IN_CODING				11<<8

	#define IN_DATA_FORMAT_1_S		0<<6
	#define IN_DATA_FORMAT_2_S		1<<6
	#define IN_DATA_FORMAT_S_M		2<<6
	#define IN_DATA_FORMAT			3<<6

	#define IN_SAMPLE_SIZE_8		0<<5
	#define IN_SAMPLE_SIZE_16		1<<5
	#define IN_SAMPLE_SIZE			1<<5

	#define LINEAR_PCM_BIT_POS		7<<2

	#define AIR_CODE_FORMAT_CVSD	0
	#define AIR_CODE_FORMAT_U		1
	#define AIR_CODE_FORMAT_A		2
	#define AIR_CODE_FORMAT		3

	

/*
***********************************************************************************************************
*											Informational Parameters
* 
***********************************************************************************************************
*/

#define 		OGF_INFO_PARAM				0x04

#define 		OCF_READ_LOCAL_VERSION		0x0001
typedef	__packed struct{
	UINT8		status;
	UINT8		hci_ver;
	UINT16		hci_rev;
	UINT8		lmp_ver;
	UINT16		man_name;
	UINT16		lmp_sub;
}read_local_version_rp;
#define		OCF_READ_LOCAL_VER_RP_SIZE	0x09
#define 		OCF_READ_LOCAL_FEATURES		0x0003
typedef __packed struct{
	UINT8		status;
	UINT8		features[8];
}read_local_features_rp;
#define	READ_LOCAL_FEATURES_RP_SIZE			0x09

#define 		OCF_READ_BUFFER_SIZE		0x0005
typedef	__packed struct{
	UINT8		status;
	UINT16		acl_mtu;
	UINT8		sco_mtu;
	UINT16		acl_max_pkt;
	UINT16		sco_max_pkt;
}read_buffer_size_rp;
#define	READ_BUFFER_SIZE_RP_SIZE			0x08

#define	 		OCF_READ_BD_ADDR			0x0009
typedef __packed struct{
	UINT8		status;
	UINT8 		bdaddr[6];
}read_bd_addr_rp;
#define READ_BD_ADDR_RP_SIZE				0x07

/*
***********************************************************************************************************
*											status parameter
* 
***********************************************************************************************************
*/

#define			OGF_STATUS_PARAM			0x05

/*
***********************************************************************************************************
*											testing commands
* 
***********************************************************************************************************
*/

#define			OGF_TEST_CMD				0x06

#define			OCF_READ_LOOPBACK_MODE		0x0001
typedef __packed struct{
	UINT8		status;
	UINT8		loopback_mode;
}read_loopback_mode_rp;
#define			OCF_WRITE_LOOOPBACK_MODE	0x0002
#define			OCF_EN_DEV_UNDER_TEST_MODE	0x0003

/*
***********************************************************************************************************
*											ericsson special command
* 
***********************************************************************************************************
*/

#define			OGF_ERICSSON_COMMAND		0x3F
#define			OCF_ERICSSON_SET_UART_BAUDRATE	0x0009
	#define BAUDRATE_300			0x19
	#define BAUDRATE_600			0x18
	#define BAUDRATE_1200			0x17
	#define BAUDRATE_2400			0x16
	#define BAUDRATE_4800			0x15
	#define BAUDRATE_9600			0x14
	#define BAUDRATE_19200			0x13
	#define BAUDRATE_38400			0x12
	#define BAUDRATE_76800			0x11
	#define BAUDRATE_153600			0x10
	#define BAUDRATE_900			0x09
	#define BAUDRATE_1800			0x08
	#define BAUDRATE_3600			0x07
	#define BAUDRATE_7200			0x06
	#define BAUDRATE_14400			0x05
	#define BAUDRATE_28800			0x04
	#define BAUDRATE_57600			0x03
	#define BAUDRATE_115200			0x02
	#define BAUDRATE_230400			0x01
	#define BAUDRATE_460800			0x00
	#define BAUDRATE_DEFAULT		0x03
					/*******************************************************/
					/******************	   HCI Events  	********************/
					/*******************************************************/

#define 		EVT_INQUIRY_COMPLETE 		0x01
#define 		EVT_INQUIRY_RESULT 			0x02
typedef __packed struct {
	UINT8		bdaddr[6];
	UINT8		pscan_rep_mode;
	UINT8		pscan_period_mode;
	UINT8		pscan_mode;
	UINT8		class[3];
	UINT16		clock_offset;
}inquiry_info;
#define 		INQUIRY_INFO_SIZE 			14
#define 		EVT_CONN_COMPLETE 			0x03
typedef __packed struct {
	UINT8		status;
	UINT16		handle;
	UINT8		bdaddr[6];
	UINT8		link_type;
	UINT8		encr_mode;
}evt_conn_complete;
#define 		EVT_CONN_COMPLETE_SIZE 		13
#define 		EVT_CONN_REQUEST			0x04
typedef __packed struct {
	UINT8 		bdaddr[6];
	UINT8 		dev_class[3];
	UINT8		link_type;
}evt_conn_request;
#define EVT_CONN_REQUEST_SIZE				10

#define 		EVT_CONN_REQUEST_SIZE 		10
#define 		EVT_DISCONN_COMPLETE		0x05
typedef __packed struct {
	UINT8 		status;
	UINT16 		handle;
	UINT8 		reason;
}evt_disconn_complete;
#define 		EVT_DISCONN_COMPLETE_SIZE 	4
#define 		EVT_CMD_COMPLETE 			0x0e
typedef __packed struct {
	UINT8 		ncmd;
	UINT16 		opcode;
}evt_cmd_complete;
#define 		EVT_CMD_COMPLETE_SIZE 		3
#define 		EVT_CMD_STATUS 				0x0f
typedef __packed struct {
	UINT8 		status;
	UINT8 		ncmd;
	UINT16 		opcode;
}evt_cmd_status;
#define 		EVT_CMD_STATUS_SIZE 		4
#define			EVT_FLUSH_OCCURRED			0x11
#define 		EVT_NUM_COMP_PKTS			0x13
typedef __packed struct {
	UINT8 		num_hndl;
	/* variable lenght part */
	//for the point to point bt device,num_hndl = 1
	UINT8		handle;
	UINT16		num_of_complete_pkts;
}evt_num_comp_pkts;
#define 		EVT_NUM_COMP_PKTS_SIZE 		4
#define			EVT_READ_CLK_OFFSET_COMPLETE 0x1C
typedef __packed struct{
	UINT8		status;
	UINT16		handle;
	UINT16		clk_offset;
}evt_read_clk_offset_complete;
#define 		EVT_HCI_DEV_EVENT			0xfd
typedef __packed struct {
	UINT16 		event;
	UINT16 		param;
}evt_hci_dev_event;
#define 		EVT_HCI_DEV_EVENT_SIZE 		4


					/*******************************************************/
					/****************HCI Packet structures******************/
					/*******************************************************/

/*
***********************************************************************************************************
*											hci command handle
* 
***********************************************************************************************************
*/

#define 		HCI_TYPE_LEN				1
typedef __packed struct {
	UINT16 		opcode;		// OCF & OGF
	UINT8 		plen;		//parameter length
}hci_command_hdr;
#define 		HCI_COMMAND_HDR_SIZE 		3

/*
***********************************************************************************************************
*											hci event handle
* 
***********************************************************************************************************
*/

typedef __packed struct {
	UINT8 		evt;		//event code
	UINT8 		plen;
}hci_event_hdr;
#define 		HCI_EVENT_HDR_SIZE 			2

/*
***********************************************************************************************************
*											hci acl data handle
* 
***********************************************************************************************************
*/

typedef __packed struct {
	UINT16 		handle;		/* Handle & Flags(PB, BC) */
	UINT16 		dlen;
}hci_acl_hdr;
#define 		HCI_ACL_HDR_SIZE 			4

/*
***********************************************************************************************************
*											hci sco data handle
* 
***********************************************************************************************************
*/

typedef __packed struct {
	UINT16 		handle;	
	UINT8 		dlen;
}hci_sco_hdr;
#define 		HCI_SCO_HDR_SIZE 			3

/*
***********************************************************************************************************
*											MACRO: Command opcode pack/unpack
* 
***********************************************************************************************************
*/

#define 		cmd_opcode_pack(ogf, ocf)	(UINT16)((ocf & 0x03ff)|(ogf << 10)) 	//pack macro
#define 		cmd_opcode_ogf(op)			(op >> 10)								//unpack macro
#define 		cmd_opcode_ocf(op)			(op & 0x03ff)							//unpack macro

/*
***********************************************************************************************************
*											MACRO: ACL handle and flags pack/unpack
* 
***********************************************************************************************************
*/

#define 		acl_handle_pack(h, f)		(UINT16)((h & 0x0fff)|(f << 12))		//pack macro
#define 		acl_handle(h)				(h & 0x0fff)							//unpack macro
#define 		acl_flags(h)				(h >> 12)								//unpack macro
    
//Above define the low_level data structures and maros.
//The following defined the device important informations and flags needed during the program operat

//device status structer.used in the future design
__packed struct  hci_dev_stats {
	UINT32 err_rx;
	UINT32 err_tx;
	UINT32 cmd_tx;
	UINT32 evt_rx;
	UINT32 acl_tx;
	UINT32 acl_rx;
	UINT32 sco_tx;
	UINT32 sco_rx;
	UINT32 byte_rx;
	UINT32 byte_tx;
};

//version information
typedef __packed struct {
	UINT8	HCI_version;		//local device hci version
	UINT16	HCI_reversion;		//local device hci reversion
	UINT8	LMP_version;		//local LMP version
	UINT16	manu_name;			//local device manufacture name
	UINT16	LMP_subversion;		//LMP subversion
}version;

//local device information
typedef __packed struct  {
	char  	name[248];//local name,in the bluetooth,the name is a UTF-8 encoded string
	UINT8	class_device[3];//class of device,noncategoried class, filled with 00 00 00
	UINT8 	bdaddr[6];//local bluetooth address
	UINT8  	features[8];//LMP_features

	//version
	version ver;
	
	UINT32 	pkt_type;//packet types,derived from the LMP_features
    //the following info get from the read buffer size cmd
    //Maximum length of the data portion of each HCI ACL Data Packet
    //that the Host Controller is able to accept.
	UINT16 	acl_mtu;
	//Total number of HCI ACL Data Packets that can be stored in 
	//the data buffers of the Host Controller.
	UINT16	acl_max;
	//Maximum length of the data portion of each HCI SCO Data Packet
	//that the Host Controller is able to accept.
	UINT16 	sco_mtu;
	//Total number of HCI SCO Data Packets that can be stored in 
	//the data buffers of the Host Controller.
	UINT16 	sco_max;
    
	struct hci_dev_stats stat;
}hci_dev_info;

//connection header information
typedef __packed struct {
	UINT16 	handle;
	UINT8   dst[6];
}hci_conn_info;

//acl connection information
typedef __packed struct {
	UINT16  link_type;
	UINT8	acl_link_flags;
	UINT16  pkt_type;
	int 		sent;
}acl_conn;

//sco connection information
typedef __packed struct {
	UINT16	link_type;
	UINT8	sco_link_flags;
	UINT16	pkt_type;
}sco_conn;

//hci connection
typedef __packed struct {
	hci_conn_info hci_c_info;
	acl_conn	  acl_conn_info;
	sco_conn	  sco_conn_info[2];
	
}hci_conn;

__packed struct hci_dev{
	
	SD_PORT			*port;//指向串口设备的指针
	//hci信息定义，分三类
	hci_dev_info	hci_device_info;//本地设备信息
	inquiry_info	hci_inquiry_info;//查询设备信息
	hci_conn 		hci_connection;//连接设备信息
	//hci信息定义
	
	inquiry_cp		ic;
	//初始化时需要设置的，开始
	//超时设置
	UINT16			conn_accept_timeout;//接受连接请求超时设置
	UINT16			page_timeout;//建立连接超时设置
	
	//enable or disable
	UINT8 			scan_enable;//扫描使能
	UINT8			authen_enable;//认证使能

	UINT16			voice_setting;//语音设置
	
	//初始化时需要设置的，结束
	
	UINT8			role;		//角色设置
	UINT16			clock_offset;//始终偏移
	
	volatile int		cmd_cnt;//发送命令的数目（上层协议兼容使用）
	unsigned int 		acl_cnt;//发送acl报文的数目（上层协议兼容使用）
	unsigned int		sco_cnt;//（上层协议兼容使用）

	
	
	UINT8 			req_status;//request状态标志
	UINT8			req_result;//request执行结果
	//Result flags
	UINT8			auth_flags;//认真标志
	UINT8 			scan_flags;//扫描标志
	UINT8			inqu_flags;//查询标志
	UINT8			link_flags;//连接标志
	UINT8			cmd_complete_flags;//命令完成标志
	UINT8			cmd_status_flags;//命令实践标志
	//发送缓冲区设置
	UINT8           		HCI_tx_buffer[256];
    INT32           		HCI_tx_buffer_read;
    INT32           		HCI_tx_buffer_write;
    volatile INT32  	HCI_tx_buffer_status;

};

//hci buffer status
#define HCI_TX_BUFF_FULL 3
#define HCI_TX_BUFF_DATA 2
#define HCI_TX_BUFF_EMPTY 1

/* ----- HCI requests ----- */
#define HCI_REQ_DONE	  0
#define HCI_REQ_PEND	  1
#define HCI_REQ_CANCELED  2


//The used const varibals,and should be initialized before
#define REJECT_REASON 0x02

//Define Default Hci Driver Settings for this System
#define DEFAULT_HCI_CONN_ACP_TIMEOUT			0xffff
#define DEFAULT_HCI_PAGE_TIMEOUT				0xffff
#define DEFAULT_HCI_SCAN_ENABLE				IS_ENA_PS_ENA
#define DEFAULT_HCI_AUTHEN_ENABLE			AUTH_DISABLED
#define DEFAULT_HCI_VOICE_SETTING_MSK		0x3c3
#define DEFAULT_HCI_VOICE_SETTING				0x141
#define DEFAULT_HCI_CLASS_OF_DEVICE			0x000000
#define DEFAULT_HCI_NAME
#define DEFAULT_HCI_IAC_LAP					0x9e8b33
#define DEFAULT_HCI_IAC_LEN					0x0a
#define DEFAULT_HCI_IAC_NUM_RSP				0x1

#endif   //__HCI_H