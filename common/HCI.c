/*
********************************************************************************
*                                               
*                       Embedded Systems Building Blocks                                 
*                    Complete and Ready-to-use Modules in C
*                                                         
*                                 HCI DRIVERS  	                                   
*                                                                                                    
*                  (c) Copyright 2004,chenna,watertek Shenzhen                              
*                             All Rights Reserved                                           
*
*Filename   : hci.c
*Programmer : chenna
********************************************************************************
*                                 DESCRIPTION
* 
********************************************************************************
*/

/*
********************************************************************************
*                                INCLUDE FILES
********************************************************************************
*/

#include "HCI.H"
#include "sd_extr.h"
#include "sd_defs.h"
/*
********************************************************************************
*                               LOCAL CONSTANTS
********************************************************************************
*/


/*
********************************************************************************
*                               GLOBAL VARIABLES
********************************************************************************
*/


/*
********************************************************************************
*                          LOCAL FUNCTIONS PROTOTYPES
********************************************************************************
*/


/*
********************************************************************************
*                         GLOBAL FUNCTIONS PROTOTYPES
********************************************************************************
*/


/*
********************************************************************************
*    					   
*
*Description :
*Arguments   :
*Retruns     :
********************************************************************************
*/
/*
********************************************************************************
*										把指定长度的数据从源地址拷贝到目的地址
* 功	能：把指定长度的数据从源地址拷贝到目的地址
********************************************************************************
*/

int __bt_mem_cpy(void * dst,void *src,int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		*((char*)dst+i) = *((char*)src+i);
	}
	return 0;

}

/*
********************************************************************************
*										地址的比较
* 功	能：比较两断内存的地址是否相等，如果相等就返回0，否则返回-1；
********************************************************************************
*/

int __bt_mem_cmp(void* buff1,void* buff2,int length)
{
	int i;
	for(i=0;i<length;i++)
	{
		if(!(*((char*)buff1+i) == (*((char*)buff2+i))))
			return -1;
	}
	return 0;
}


/*
********************************************************************************
*										将某块内存设置成指定的数据
* 功	能：将以buff为起始的count个字节设置为数据ch
********************************************************************************
*/

void __bt_mem_set(void *buff,char ch,unsigned count)
{
	int i;
	for(i =0;i<count;i++)
	{
		*((char *)(buff)+i) =(char) ch;
	}
	
}


/*
********************************************************************************
*										大小模式的转换
* 功	能：实现半字的高低两byte数据的交换，即实现大小模式的转换
********************************************************************************
*/

UINT16 __host_to_hci_16(UINT16 i)
{
	//This function is used when the host and hci communications a 2 bytes logic block.this function can covert the byte order to the correct
	/*
	int temp;
	temp = (i&0xff)<<8;
	i =temp| (i>>8);*/
	return i;
}

/*
********************************************************************************
*										大小模式的转换
* 功	能：实现半字的高低两byte数据的交换，即实现大小模式的转换
********************************************************************************
*/

UINT16 __hci_to_host_16(UINT16 i)
{
	i =__host_to_hci_16(i);
	return i;
}


/*
********************************************************************************
*										输出
* 功	能：将HCI_Tx_buff[1024]里面的内容输出出来，通过AXD的semihosting功能，输出在AXD的console控制台里面
********************************************************************************
*/
__inline int HCI_Put_char(struct hci_dev *hdev,char ch)
{
	int new_write;
	new_write=hdev->HCI_tx_buffer_write+1;
	if(new_write>=256)
	{
		new_write = 0;
	}
	if(new_write==hdev->HCI_tx_buffer_read)
	{
		hdev->HCI_tx_buffer_status=HCI_TX_BUFF_FULL;
		return HCI_TX_BUFF_FULL;
	}
	hdev->HCI_tx_buffer[hdev->HCI_tx_buffer_write]=ch;
	hdev->HCI_tx_buffer_write=new_write;
	return HCI_TX_BUFF_DATA;	
}
__inline unsigned char HCI_Get_char(struct hci_dev *hdev)
{
	
	unsigned char data;
	if(hdev->HCI_tx_buffer_read != hdev->HCI_tx_buffer_write)
	{
		data = hdev->HCI_tx_buffer[hdev->HCI_tx_buffer_read++];
		if(hdev->HCI_tx_buffer_read==256)
		{
			hdev->HCI_tx_buffer_read=0;
		}
		if(hdev->HCI_tx_buffer_read==hdev->HCI_tx_buffer_write)
		{
			hdev->HCI_tx_buffer_status = HCI_TX_BUFF_EMPTY;
		}else
		{
			hdev->HCI_tx_buffer_status = HCI_TX_BUFF_DATA;
		}
	}
	return data;
}
__inline void __bt_print_to_sdc(struct hci_dev *hdev)
{
	unsigned char ch;
	while(hdev->HCI_tx_buffer_read != hdev->HCI_tx_buffer_write)
	{
		ch = HCI_Get_char(hdev);
	#ifdef DEBUG
		printf("%x ",ch);
	#else
		SDC_Put_Char(ch, hdev->port);
	#endif
	//	hdev->HCI_tx_buffer_read++;
	}
	
}

/*
********************************************************************************
*										延时
* 功	能：延时
********************************************************************************
*/
void __delay(int counter)
{
	int i;
	for(i=0;i<5000*counter;i++)
	{}
}
/*
__inline void hci_req_complete(struct hci_dev *hdev, int result)
{
	if (hdev->req_status == HCI_REQ_PEND) {
		hdev->req_result = result;
		hdev->req_status = HCI_REQ_DONE;
	}

}

*/
/*
********************************************************************************
    							HCI Commands
********************************************************************************
*/

/*
***********************************************************************************************************
*										发送命令
* 功	能：蓝牙发送命令是以小模式来发送的，按照byte为单位，从lsb开始发送，msb发送在最后。
*			这个函数把要发送的命令及其参数按照小模式的格式发再HCI_Tx_buff里面，等待发送。
***********************************************************************************************************
*/

void hci_send_cmd(struct hci_dev *hdev,UINT16 ogf,UINT8 ocf,UINT32 plen,void *param)
{
	int i;
	hci_command_hdr hc;
	//int len = 1 + HCI_COMMAND_HDR_SIZE + plen;//add the pkt indicator
	hc.opcode = cmd_opcode_pack(ogf,ocf);	/* remember to invert the order*/
	hc.plen = plen;
	//tx the packet indicator
	HCI_Put_char(hdev,(unsigned char)HCI_COMMAND_PKT);
	//tx the command hdr 
	for(i=0;i<HCI_COMMAND_HDR_SIZE;i++)
		HCI_Put_char(hdev,*((unsigned char*)&hc+i));
	//put the parameter
	for(i=0;i<plen;i++)
		HCI_Put_char(hdev,*((char*)param+i));
}
/*
***********************************************************************************************************
*										hci_inquiry
* 功	能：发送inquiry命令，查询周围的蓝牙设备
***********************************************************************************************************
*/

void hci_inq_req(struct hci_dev *hdev)
{
 	inquiry_cp *ic;
  	/* Start Inquiry */
  	//hdev->req_status = HCI_REQ_PEND;
	ic = &(hdev->ic);
  	hci_send_cmd(hdev, OGF_LINK_CTL, OCF_INQUIRY, INQUIRY_CP_SIZE, (void*)ic);
}
/*
***********************************************************************************************************
*										hci_conn_add
* 功	能：将与指定的蓝牙地址的蓝牙设备的连接信息添加到hci_conn_array[4]里面
***********************************************************************************************************
*/

void hci_conn_add(struct hci_dev *hdev,UINT16 handle,UINT8 type,char *dst)
{
	
	int i;
	if(hdev->hci_connection.hci_c_info.handle==handle||hdev->hci_connection.hci_c_info.handle==NULL)
	{
		hdev->hci_connection.hci_c_info.handle=handle;
		__bt_mem_cpy((void*)hdev->hci_connection.hci_c_info.dst,(void*)dst,6);
		if(type)//acl link
		{
			hdev->hci_connection.acl_conn_info.link_type = type;
			hdev->hci_connection.acl_conn_info.acl_link_flags = 1;
			hdev->hci_connection.sco_conn_info[0].sco_link_flags = 0;
			hdev->hci_connection.sco_conn_info[1].sco_link_flags = 0;
		}
		else{
			for(i=0;i<2;i++)
				if(!(hdev->hci_connection.sco_conn_info[i].sco_link_flags))
				{
					hdev->hci_connection.sco_conn_info[i].link_type = type;
					hdev->hci_connection.sco_conn_info[i].sco_link_flags = 1;
					return;
				}
		}
	}
}

void hci_conn_del(struct hci_dev *hdev)
{
	hdev->hci_connection.hci_c_info.handle=NULL;
	hdev->hci_connection.sco_conn_info[0].sco_link_flags = 0;
	hdev->hci_connection.sco_conn_info[1].sco_link_flags = 0;
}

/*
***********************************************************************************************************
*										hci create connection
* 功	能：发送创建连接命令命令，与指定蓝牙地址的蓝牙设备建立连接
***********************************************************************************************************
*/

//hci command:hci create connection
int hci_create_connect(struct hci_dev *hdev,char *bdaddr)
{
	create_conn_cp cc;
	UINT16 clock_offset;
	cc.pscan_rep_mode = hdev->hci_inquiry_info.pscan_rep_mode;
	cc.pscan_mode = hdev->hci_inquiry_info.pscan_mode;
	clock_offset = (hdev->hci_inquiry_info.clock_offset) & 0x8000;
	__bt_mem_cpy((void*)&cc.bdaddr,(void*)bdaddr,6);
	cc.pkt_type = hdev->hci_device_info.pkt_type;
	cc.clock_offset = clock_offset;
	cc.role_switch = (hdev->hci_device_info.features[0] & LMP_RSWITCH) ? 0x01:0x00;
	hci_send_cmd(hdev,OGF_LINK_CTL,OCF_CREATE_CONN,CREATE_CONN_CP_SIZE,(void*)&cc);
	return 0;
}

/*
***********************************************************************************************************
*										hci disconnect
* 功	能：发送断开连接命令命令，并给出断开连接的原因
***********************************************************************************************************
*/

int hci_disconnect(struct hci_dev *hdev,UINT8 reason)
{
	disconnect_cp dc;
	dc.handle = (UINT16)(hdev->hci_connection.hci_c_info.handle);
	dc.reason = reason;
	hci_send_cmd(hdev,OGF_LINK_CTL,OCF_DISCONNECT,DISCONNECT_CP_SIZE,(void*)&dc);
	return 0;
}


/*
***********************************************************************************************************
*										add sco connection
* 功	能：发送添加sco连接命令，创建sco连接
***********************************************************************************************************
*/

int hci_add_sco_conn(struct hci_dev *hdev)
{
	add_sco_con_cp asc;
	asc.handle = (UINT16)(hdev->hci_connection.hci_c_info.handle);
	asc.pkt_type = hdev->hci_device_info.pkt_type;
	hci_send_cmd(hdev,OGF_LINK_CTL,OCF_ADD_SCO_CONN,ADD_SCO_CONN_SIZE,(void*)&asc);
	return 0;
}

/*
***********************************************************************************************************
*										hci accept connnection requst
* 功	能：发送接收远方蓝牙设备建立连接命令
***********************************************************************************************************
*/

int hci_accept_conn_req(struct hci_dev *hdev)
{
	accept_conn_req_cp acr;
	hdev->req_status = HCI_REQ_PEND;
	__bt_mem_cpy((void*)(acr.bdaddr),(void*)(hdev->hci_inquiry_info.bdaddr),6);//
	acr.role = (hdev->hci_device_info.features[0] & LMP_RSWITCH) ? 0x01:0x00;
	hci_send_cmd(hdev,OGF_LINK_CTL,OCF_ACCEPT_CONN_REQ,ACCEPT_CONN_REQ_CP_SIZE,(void*)&acr);
	return 0;
}

/*
***********************************************************************************************************
*										拒绝建立连接请求
* 功	能：发送拒绝建立连接请求命令
***********************************************************************************************************
*/

int hci_reject_conn_req(struct hci_dev *hdev,UINT8 reason)
{
	reject_conn_cp rc;
	hdev->req_status = HCI_REQ_PEND;
	__bt_mem_cpy((void*)(rc.bdaddr),(void*)(hdev->hci_connection.hci_c_info.dst),6);//?to be corrected
	rc.reason = reason;
	hci_send_cmd(hdev,OGF_LINK_CTL,OCF_REJECT_CONN_REQ,REJECT_CONN_REQ_CP_SIZE,(void*)&rc);
	return 0;
}

/*
***********************************************************************************************************
*										请求远方设备的friendly name
* 功	能：发送请求远方设备的friendly name命令
***********************************************************************************************************
*/

int hci_remote_name_req(struct hci_dev *hdev)
{
	remote_name_req_cp rnr;
	hdev->req_status = HCI_REQ_PEND;
	__bt_mem_cpy((void*)(rnr.bdaddr),(void*)(hdev->hci_connection.hci_c_info.dst),6);
	rnr.pscan_rep_mode = hdev->hci_inquiry_info.pscan_rep_mode;
	rnr.pscan_mode = hdev->hci_inquiry_info.pscan_mode;
	rnr.clock_offset = (hdev->hci_inquiry_info.clock_offset) & 0x8000;
	hci_send_cmd(hdev,OGF_LINK_CTL,OCF_REMOTE_NAME_REQ,REMOTE_NAME_REQ_CP_SIZE,(void*)&rnr);
	return 0;
}

/*
***********************************************************************************************************
*										hci scan request
* 功	能：发送scan命令
***********************************************************************************************************
*/

void hci_scan_req(struct hci_dev *hdev)
{
	UINT8 scan = hdev->scan_enable;
	hdev->req_status = HCI_REQ_PEND;
	//Inquiry and Page scans
	hci_send_cmd(hdev,OGF_HOST_CTL,OCF_WRITE_SCAN_ENABLE,1,&scan);
}

/*
***********************************************************************************************************
*										hci authentication request
* 功	能：发送authentication命令
***********************************************************************************************************
*/

void hci_auth_req(struct hci_dev *hdev)
{
	UINT8 auth = hdev->authen_enable;
	hdev->req_status = HCI_REQ_PEND;
 	/* Authentication */
  	hci_send_cmd(hdev, OGF_HOST_CTL, OCF_WRITE_AUTH_ENABLE, 1, &auth);
}

/*
   *************************************************************************
    							HOST CONTROL
   *************************************************************************
*/

/*
***********************************************************************************************************
*										hci reset request
* 功	能：发送reset命令
***********************************************************************************************************
*/

void hci_reset_req(struct hci_dev *hdev)
{
	hdev->req_status = HCI_REQ_PEND;
	hci_send_cmd(hdev, OGF_HOST_CTL, OCF_RESET, 0, NULL);
}

/*
   *************************************************************************
    							PKT ANALYZE FUNCTION
   *************************************************************************
*/
__inline void hci_req_complete(struct hci_dev *hdev, int result)
{
	if (hdev->req_status == HCI_REQ_PEND) {
		hdev->req_result = result;
		hdev->req_status = HCI_REQ_DONE;
	}

}
__inline void hci_cmd_complete(struct hci_dev *hdev, int result)
{
	if (hdev->req_status == HCI_REQ_PEND) {
		hdev->cmd_complete_flags= result;
		hdev->req_status = HCI_REQ_DONE;
	}

}
/*
***********************************************************************************************************
*										 Command Complete OGF LINK_CTL  
* 功	能：
***********************************************************************************************************
*/
/* Command Complete OGF LINK_POLICY  */
void hci_cc_link_policy(struct hci_dev *hdev, UINT16 ocf)
{
	#ifdef DEBUG
	printf("%s ocf 0x%x", hdev->hci_device_info.name, ocf);
	#endif
	switch (ocf) {
	default:
	{
		//UINT8 status;
		//status=SDC_Get_Char(hdev->port);
		//if(!status)
		//{
		//	hdev->cmd_complete_flags = 1;
			#ifdef DEBUG
			printf("%s Command complete: ogf LINK_CTL ocf %x", hdev->name, ocf);
			#endif
		//}
		break;
	}
	}
}

//In this function the main handling is to be filled
void hci_cc_link_ctl(struct hci_dev *hdev, UINT16 ocf)
{
	switch (ocf) {
	default:
		{
			UINT8 status;
			status=SDC_Get_Char(hdev->port);
			if(!status)
			{
				hdev->cmd_complete_flags = 0;
				#ifdef DEBUG
				printf("%s Command complete: ogf LINK_CTL ocf %x", hdev->hci_device_info.name, ocf);
				#endif
			}
		}
		break;
	}
}
/* Command Complete OGF HOST_CTL  */
void hci_cc_host_ctl(struct hci_dev *hdev,UINT16 ocf)
{
	UINT8 status,param;
	//void *send;
	//printf("%s ocf 0x%x",hdev->name,ocf);
	switch(ocf){
	case OCF_EVT_MSK:
		
		break;
	case OCF_RESET:
		status =(UINT8)(SDC_Get_Char(hdev->port));//replaced form line1359
		hci_cmd_complete(hdev,status);
		break;
	case OCF_SET_EVENT_FLT:
		status = (UINT8)(SDC_Get_Char(hdev->port));//replaced form line
		hci_cmd_complete(hdev,status);
		#ifdef DEBUG
		if(status)
			printf("%s SET_EVENT_FLT failed %d", hdev->name, status);
		else
			printf("%s SET_EVENT_FLT successful", hdev->name);
		#endif
		break;
	
	case OCF_WRITE_AUTH_ENABLE:
		//if (!(send = hci_send_cmd_data(hdev, OGF_HOST_CTL, OCF_WRITE_AUTH_ENABLE)))
			//break;
		status =(UINT8)(SDC_Get_Char(hdev->port));			//????????????????????????????????????????????????
		param = hdev->authen_enable;
		
		if(!status){
			if(param == AUTH_ENABLED)
				hdev->auth_flags |= HCI_AUTH;
			else
				hdev->auth_flags &= ~HCI_AUTH;
		}
		hci_cmd_complete(hdev,status);
		break;
	case OCF_WRITE_CA_TIMEOUT:
		status = (UINT8)(SDC_Get_Char(hdev->port));			//????????????????????????????????????????????????
		hci_cmd_complete(hdev,status);
		#ifdef DEBUG
		if(status){
			printf("%s OCF_WRITE_CA_TIMEOUT failed %d", hdev->name, status);
		}else{
			printf("%s OCF_WRITE_CA_TIMEOUT succeseful", hdev->name);
		}
		#endif
		break;
	case OCF_WRITE_PG_TIMEOUT:
		status =(UINT8)(SDC_Get_Char(hdev->port));
		#ifdef DEBUG
		if(status){
			printf("%s OCF_WRITE_PG_TIMEOUT failed %d", hdev->name, status);
		}else{
			printf("%s OCF_WRITE_PG_TIMEOUT succeseful", hdev->name);
		}
		#endif
		break;
	case OCF_WRITE_SCAN_ENABLE:
		status = (UINT8)(SDC_Get_Char(hdev->port));			//????????????????????????????????????????????????
		param = hdev->scan_enable;
		if(!status){
			switch(param){
			case IS_ENA_PS_ENA:
				hdev->scan_flags |=  HCI_PSCAN | HCI_ISCAN;
				break;
			case IS_ENA_PS_DIS:
				hdev->scan_flags &= ~HCI_PSCAN;
				hdev->scan_flags |=  HCI_ISCAN;
				break;
			case IS_DIS_PS_ENA:
				hdev->scan_flags &= ~HCI_ISCAN;
				hdev->scan_flags |=  HCI_PSCAN;
				break;
			default:
				hdev->scan_flags &= ~(HCI_ISCAN | HCI_PSCAN);
				break;
			};
		}
		hci_cmd_complete(hdev, status);
		break;
	default:
		#ifdef DEBUG
		printf("%s Command complete: ogf HOST_CTL ocf %x", hdev->hci_device_info.name, ocf);
		#endif
		break;
	};
}

/* Command Complete OGF INFO_PARAM  */
void hci_cc_info_param(struct hci_dev *hdev,UINT16 ocf)
{

 //READ_BUFFER_SIZE_RP_SIZE READ_BD_ADDR_RP_SIZE
	read_local_features_rp lf;
	read_buffer_size_rp bs;
	read_bd_addr_rp ba;
	read_local_version_rp lv;
	int i;
	switch(ocf){
	case OCF_READ_LOCAL_VERSION:
		for(i=0;i<OCF_READ_LOCAL_VER_RP_SIZE;i++)
		{
			if((hdev->port->rx_buffer_status==SDC_BUFFER_DATA)||(hdev->port->rx_buffer_status==SDC_BUFFER_FULL))
			{
				*((UINT8*)(&lv)+i) = (UINT8)(SDC_Get_Char(hdev->port));
			}
		}
		if(lv.status)
		{
			hci_cmd_complete(hdev, lv.status);
			#ifdef DEBUG
			printf("%s READ_LOCAL_VERSION failed %d", hdev->hci_device_info.name, lv.status);
			#endif
			break;
		}
		__bt_mem_cpy((void *)(&( hdev->hci_device_info.ver)), (void *)((char*)(&lv)+1), OCF_READ_LOCAL_VER_RP_SIZE-1);
		break;
	case OCF_READ_LOCAL_FEATURES:
		for(i=0;i<READ_LOCAL_FEATURES_RP_SIZE;i++)
		{
			if((hdev->port->rx_buffer_status==SDC_BUFFER_DATA)||(hdev->port->rx_buffer_status==SDC_BUFFER_FULL))
			{
				*((UINT8*)(&lf)+i) = (UINT8)(SDC_Get_Char(hdev->port));
			}
		}
		if(lf.status){
			hci_cmd_complete(hdev, lf.status);
			#ifdef DEBUG
			printf("%s READ_LOCAL_FEATURES failed %d", hdev->name, lf.status);
			#endif
			break;
		}
		__bt_mem_cpy((void*)(hdev->hci_device_info.features), (void*)lf.features, 8);
		/* Adjust default settings according to features supported by device. */
		if(hdev->hci_device_info.features[0] &LMP_3SLOT)
			hdev->hci_device_info.pkt_type |= (HCI_DM3 | HCI_DH3);
		if (hdev->hci_device_info.features[0] & LMP_5SLOT)
			hdev->hci_device_info.pkt_type |= (HCI_DM5 | HCI_DH5);
		#ifdef DEBUG
		printf("%s: features 0x%x 0x%x 0x%x", hdev->hci_device_info.name, lf.features[0], lf.features[1], lf.features[2]);
		#endif
		break;
	case OCF_READ_BUFFER_SIZE:
		for(i=0;i<READ_BUFFER_SIZE_RP_SIZE;i++)
		{
			if((hdev->port->rx_buffer_status==SDC_BUFFER_DATA)||(hdev->port->rx_buffer_status==SDC_BUFFER_FULL))
			{
				*((UINT8*)(&bs)+i) = (UINT8)(SDC_Get_Char(hdev->port));
			}
		}
		if (bs.status) {
			hci_cmd_complete(hdev, bs.status);
			#ifdef DEBUG
			printf("%s READ_BUFFER_SIZE failed %d", hdev->hci_device_info.name, bs.status);
			#endif
			break;
		}
		hdev->hci_device_info.acl_mtu = (UINT16)(bs.acl_mtu);
		hdev->hci_device_info.sco_mtu = (UINT8 )(bs.sco_mtu);
		hdev->hci_device_info.acl_max = hdev->acl_cnt = (UINT16)(bs.acl_max_pkt);
		hdev->hci_device_info.sco_max = hdev->sco_cnt = (UINT16)(bs.sco_max_pkt);
		#ifdef DEBUG
		printf("%s mtu: acl %d, sco %d max_pkt: acl %d, sco %d", hdev->hci_device_info.name,hdev->hci_device_info.acl_mtu, hdev->hci_device_info.sco_mtu, hdev->hci_device_info.acl_max, hdev->hci_device_info.sco_max);
		#endif
		break;
	case OCF_READ_BD_ADDR:
		for(i=0;i<READ_BD_ADDR_RP_SIZE;i++)
		{
			if((hdev->port->rx_buffer_status==SDC_BUFFER_DATA)||(hdev->port->rx_buffer_status==SDC_BUFFER_FULL))
			{
				*((UINT8*)(&ba)+i) = (UINT8)(SDC_Get_Char(hdev->port));
			}
		}
		if(!ba.status){
			__bt_mem_cpy((void*)(hdev->hci_device_info.bdaddr),(void*)(ba.bdaddr),6);
		}else{
			#ifdef DEBUG
			printf("%s: READ_BD_ADDR failed %d", hdev->hci_device_info.name, ba.status);
			#endif
		}
		hci_cmd_complete(hdev,ba.status);
		break;
	default:
		#ifdef DEBUG
		printf("%s Command complete: ogf INFO_PARAM ocf %x", hdev->hci_device_info.name, ocf);
		#endif
		break;
	}
}

/* Command Status OGF LINK_CTL  */
void hci_cs_link_ctl(struct hci_dev *hdev,UINT16 ocf,UINT8 status)
{
	switch(ocf){
	case OCF_CREATE_CONN:
		hci_req_complete(hdev,status);		//future
		if(status){
			//create_conn_cp *cc = hci_send_cmd_data(hdev,OGF_LINK_CTL,OCF_CREATE_CONN);
			//if(!cc)
			//	break;
			//printf("%s Create connection error: status 0x%x %s", hdev->name,status,&cc.bdaddr);
			
		}
		break;
	case OCF_INQUIRY:
		hci_req_complete(hdev,status);//future
		if(status){
			#ifdef DEBUG
			printf("%s Inquiry error: status 0x%x", hdev->hci_device_info.name, status);
			#endif
			
		}
		break;
	default:
		#ifdef DEBUG
		printf("%s Command status: ogf LINK_CTL ocf %x", hdev->hci_device_info.name, ocf);
		#endif
		break;
	}
}

/* Command Status OGF LINK_POLICY */
void hci_cs_link_policy(struct hci_dev *hdev,UINT16 ocf,UINT8 status)
{
	switch(ocf){
	default:
		hci_req_complete(hdev,status);
		#ifdef DEBUG
		printf("%s Command status: ogf HOST_POLICY ocf %x", hdev->hci_device_info.name, ocf);
		#endif
	}
}

/* Command Status OGF HOST_CTL */
void hci_cs_host_ctl(struct hci_dev *hdev,UINT16 ocf,UINT8 status)
{
	switch(ocf){
	default:
		hci_req_complete(hdev, status);
		#ifdef DEBUG
		printf("%s Command status: ogf HOST_CTL ocf %x", hdev->hci_device_info.name, ocf);
		#endif
	}
}

/* Command Status OGF INFO_PARAM  */
void hci_cs_info_param(struct hci_dev *hdev,UINT16 ocf,UINT8 status)
{
	switch(ocf){
	default:
		hci_req_complete(hdev, status);
		#ifdef DEBUG
		printf("%s Command status: ogf INFO_PARAM ocf %x", hdev->hci_device_info.name, ocf);
		#endif
	}
}

/* Inquiry Complete */
void hci_inquiry_complete_evt(struct hci_dev *hdev)
{
	UINT8 status =(UINT8)(SDC_Get_Char(hdev->port));
	hci_req_complete(hdev,status);
	#ifdef DEBUG
	printf("%s status %d", hdev->hci_device_info.name, status);
	#endif
}

/* Inquiry Result */
void hci_inquiry_result_evt(struct hci_dev *hdev)
{
	int i;
	int num_rsp = (UINT8)(SDC_Get_Char(hdev->port));
	if(num_rsp !=1)
		return;
	for(i = 0;i<INQUIRY_INFO_SIZE;i++)
	{
		*((UINT8 *)(&(hdev->hci_inquiry_info))+i) = (UINT8)(SDC_Get_Char(hdev->port));
	}
  	//置标志位,已经接受一个蓝牙的inquiry信息
  	hdev->inqu_flags = 0x0;
}

/* Connect Request */
void hci_conn_request_evt(struct hci_dev *hdev)
{
	int i;
	evt_conn_request cr;
	accept_conn_req_cp ac;
	reject_conn_cp rc;
	int accept = 0;
	for(i=0;i<EVT_CONN_REQUEST_SIZE;i++)
	{
		*((UINT8 *)(&cr)+i) = (UINT8)(SDC_Get_Char(hdev->port));
	}
	
	//printf("%s Connection request: %s type 0x%x", hdev->name, &cr->bdaddr, cr->link_type);
	
	if (cr.link_type == ACL_LINK) {
		/* Notify upper protocols ACL link notify L2CAP */
		
		
	}else{
	/* SCO link (no notification) */
	accept = 1;
	
	}
	if (accept) {
		/* Connection accepted by upper layer */
		__bt_mem_cpy((void*)(ac.bdaddr),(void*)(cr.bdaddr),6);
		ac.role = 0x01; //remain slave
		hci_send_cmd(hdev, OGF_LINK_CTL, OCF_ACCEPT_CONN_REQ, ACCEPT_CONN_REQ_CP_SIZE, (void*)&ac);
	}else{
		__bt_mem_cpy((void*)(rc.bdaddr),(void*)(cr.bdaddr),6);
		rc.reason = REJECT_REASON;
		hci_send_cmd(hdev,OGF_LINK_CTL,OCF_REJECT_CONN_REQ,REJECT_CONN_REQ_CP_SIZE,(void*)&rc);
	}
}

/* Connect Complete */
void hci_conn_complete_evt(struct hci_dev *hdev)
{
	int i;
	evt_conn_complete cc;
	for(i=0;i<EVT_CONN_COMPLETE_SIZE;i++)
	{
		*((UINT8 *)(&cc)+i) = (UINT8)(SDC_Get_Char(hdev->port));
	}
	if (!cc.status)
	{
		hci_conn_add(hdev,(UINT16)(cc.handle), cc.link_type, (char*)cc.bdaddr);
		hdev->link_flags=0;
		//for notify upper protocols.
		if (cc.link_type == ACL_LINK) {
	
	
		}else{
	
	
		}
	}
}

/* Disconnect Complete */

void hci_disconn_complete_evt(struct hci_dev *hdev)
{
	int i;
	UINT16 handle;
	evt_disconn_complete dc;
	for(i=0;i<EVT_DISCONN_COMPLETE_SIZE;i++)
	{
		*((char*)(&dc)+i) = (UINT8)(SDC_Get_Char(hdev->port));
	}
	handle = (UINT16)(dc.handle);
	
	if (!dc.status && (handle == hdev->hci_connection.hci_c_info.handle)) {
		// For Notify upper protocols 
		//if (conn->type == ACL_LINK) {
		
		
		//}else{
	
	
		//}
		hci_conn_del(hdev);
	}
}

/* Number of completed packets */
void hci_num_comp_pkts_evt(struct hci_dev *hdev)
{
	evt_num_comp_pkts nc;
	//UINT16 *ptr;
	int i;
	for(i=0;i<EVT_NUM_COMP_PKTS_SIZE;i++)
	{
		*((char*)(&nc)+i) = (UINT8)(SDC_Get_Char(hdev->port));
	}
	if(nc.num_hndl!=1)
	{
		//error,not a point to point bt device
		return;
	}
	if(nc.handle!=hdev->hci_connection.hci_c_info.handle)
	{
		//error,handle error,not matching
		return;
	}
	else{
		hdev->hci_connection.acl_conn_info.sent -=nc.num_of_complete_pkts;
	}
	hdev->acl_cnt +=nc.num_of_complete_pkts;
	
}

//hci event packet
void hci_event_packet(struct hci_dev *hdev)
{
	hci_event_hdr he;
	evt_cmd_status cs;
	evt_cmd_complete ec;
	UINT16 opcode,ocf,ogf;
	UINT8 indicator;
	int i;
	indicator=(UINT8)(SDC_Get_Char(hdev->port));
	if(indicator!=4)
	{
	//not a event packet,return;
		return;
	}
	for(i=0;i<HCI_EVENT_HDR_SIZE;i++)
	{
		*((char*)&he+i) = (UINT8)(SDC_Get_Char(hdev->port));
	}
	
	switch(he.evt){
	case EVT_NUM_COMP_PKTS:
			hci_num_comp_pkts_evt(hdev);
			break;
	case EVT_INQUIRY_COMPLETE:
			hci_inquiry_complete_evt(hdev);
			break;
	case EVT_INQUIRY_RESULT:
			hci_inquiry_result_evt(hdev);
			break;
	case EVT_CONN_REQUEST:
			hci_conn_request_evt(hdev);
			break;
	case EVT_CONN_COMPLETE:
			hci_conn_complete_evt(hdev);
			break;
	case EVT_DISCONN_COMPLETE:
			hci_disconn_complete_evt(hdev);
			break;
	case EVT_CMD_STATUS:
			for(i=0;i<EVT_CMD_STATUS_SIZE;i++)
			{
				*((char*)&cs+i)=(UINT8)(SDC_Get_Char(hdev->port));
			}
			opcode = (UINT16)(cs.opcode);
			ogf = cmd_opcode_ogf(opcode);
			ocf = cmd_opcode_ocf(opcode);
			switch(ogf){
			case OGF_INFO_PARAM:
					hci_cs_info_param(hdev, ocf, cs.status);
					break;
			case OGF_HOST_CTL:
					hci_cs_host_ctl(hdev, ocf, cs.status);
					break;
			case OGF_LINK_CTL:
					hci_cs_link_ctl(hdev, ocf, cs.status);
					break;
			case OGF_LINK_POLICY:
					hci_cs_link_policy(hdev, ocf, cs.status);
					break;
			default:
					//command status
					#ifdef DEBUG
					printf("%s Command Status OGF %x", hdev->hci_device_info.name, ogf);
					#endif
					break;
			}
			break;
	case EVT_CMD_COMPLETE:
			//ec = (evt_cmd_complete *)((UINT8 *)cb->Head+HCI_EVENT_HDR_SIZE);
			for(i=0;i<EVT_CMD_COMPLETE_SIZE;i++)
			{
				*((char*)&ec+i)=(UINT8)(SDC_Get_Char(hdev->port));
			}
			opcode = /*(UINT16)*/(ec.opcode);
			ogf = cmd_opcode_ogf(opcode);
			ocf = cmd_opcode_ocf(opcode);
			switch(ogf){
			case OGF_INFO_PARAM:
					hci_cc_info_param(hdev, ocf);
					break;
			case OGF_HOST_CTL:
					hci_cc_host_ctl(hdev, ocf);
					break;
			case OGF_LINK_CTL:
					hci_cc_link_ctl(hdev, ocf);
					break;
			case OGF_LINK_POLICY:
					hci_cc_link_policy(hdev, ocf);
					break;
			default:
					//command status
					#ifdef DEBUG
					printf("%s Command Completed OGF %x", hdev->hci_device_info.name, ogf);
					#endif
					break;
			}
	}
	
	//to add addix code here.
	
}


//The following is the user function interfaces
void hci_init_req1(struct hci_dev *hdev)
{
	write_class_of_dev_cp wcd;
	UINT8 name_len;
	//reset
	hci_send_cmd(hdev, OGF_HOST_CTL, OCF_RESET, 0, NULL);
	__bt_print_to_sdc(hdev);
	__delay(1);
	//Mandatory initialization
	//Read BD Address and 
	hci_send_cmd(hdev, OGF_INFO_PARAM, OCF_READ_BD_ADDR, 0, NULL);
	__bt_print_to_sdc(hdev);
	__delay(2);
	//Read Local Supported Features 
	hci_send_cmd(hdev, OGF_INFO_PARAM, OCF_READ_LOCAL_FEATURES, 0, NULL);
	__bt_print_to_sdc(hdev);
	__delay(1);
	//Read local version information
	hci_send_cmd( hdev, OGF_INFO_PARAM, OCF_READ_LOCAL_VERSION, 0, NULL);
	__bt_print_to_sdc(hdev);
	__delay(1);
	//Read Buffer Size (ACL mtu, max pkt, etc.)
	hci_send_cmd(hdev, OGF_INFO_PARAM, OCF_READ_BUFFER_SIZE, 0, NULL);
	__bt_print_to_sdc(hdev);
	__delay(1);

	while((hdev->port->rx_buffer_status==SDC_BUFFER_DATA)||(hdev->port->rx_buffer_status==SDC_BUFFER_FULL))
	{
		hci_event_packet(hdev);
	}
	//write class of device cmd
	__bt_mem_cpy((void*) (&wcd), (void * )(hdev->hci_device_info.class_device),3);
	hci_send_cmd( (void*)hdev, OGF_HOST_CTL, OCF_WRITE_CLASS_OF_DEV, WRITE_CLASS_OF_DEV_CP_SIZE,(void*)(&wcd));
	__bt_print_to_sdc(hdev);
	__delay(1);
	//write device name cmd
	for(name_len=0;*((char*)(hdev->hci_device_info.name)+name_len)!='\0';name_len++)
	{
	}
	hci_send_cmd( (void*)hdev, OGF_HOST_CTL, OCF_CHANGE_LOCAL_NAME, name_len, (void*)(hdev->hci_device_info.name));
	__bt_print_to_sdc(hdev);
	__delay(1);
	while((hdev->port->rx_buffer_status==SDC_BUFFER_DATA)||(hdev->port->rx_buffer_status==SDC_BUFFER_FULL))
	{
		hci_event_packet(hdev);
	}
}
void hci_init_req2(struct hci_dev *hdev)
{
	set_event_flt_cp ec;
	/* Optional initialization */
	//Clear Event Filters
	ec.flt_type  = FLT_CLEAR_ALL;
	hci_send_cmd((void*)hdev, OGF_HOST_CTL, OCF_SET_EVENT_FLT, 1, (void*)(&ec));
	__bt_print_to_sdc(hdev);
	__delay(1);
	//write scan enable
	hci_scan_req(hdev);
	__bt_print_to_sdc(hdev);
	__delay(1);
	//write authen disable
	hci_auth_req(hdev);
	__bt_print_to_sdc(hdev);
	__delay(1);
	//write voice setting
	hci_send_cmd((void*)hdev, OGF_HOST_CTL, OCF_WRITE_VOICE_SETTING, 2, (void*)(&(hdev->voice_setting)));
	__bt_print_to_sdc(hdev);
	__delay(1);
	//set event filter,set auto accept
	ec.flt_type = FLT_CONN_SETUP;
	ec.cond_type = CONN_SETUP_ALLOW_ALL;
	ec.condition = CONN_SETUP_AUTO_ON;
	hci_send_cmd(hdev, OGF_HOST_CTL, OCF_SET_EVENT_FLT, 3, (void*)(&ec)); 
	__bt_print_to_sdc(hdev);
	__delay(1);
	//Connection accept timeout ~20 secs
	hci_send_cmd(hdev, OGF_HOST_CTL, OCF_WRITE_CA_TIMEOUT, 2, (void*)(&(hdev->conn_accept_timeout)));
	__bt_print_to_sdc(hdev);
	__delay(1);
	//Page timeout secs
	hci_send_cmd(hdev, OGF_HOST_CTL, OCF_WRITE_PG_TIMEOUT, 2, (void*)(&(hdev->page_timeout)));
	__bt_print_to_sdc(hdev);
	__delay(1);
	while((hdev->port->rx_buffer_status==SDC_BUFFER_DATA)||(hdev->port->rx_buffer_status==SDC_BUFFER_FULL))
	{
		hci_event_packet(hdev);
	}

}

void HCI_master_create_acl_conn(struct hci_dev *hdev)
{
	hci_inq_req(hdev);
	__bt_print_to_sdc(hdev);
	__delay(10);
	while(hdev->inqu_flags)
	{
		while((hdev->port->rx_buffer_status==SDC_BUFFER_DATA)||(hdev->port->rx_buffer_status==SDC_BUFFER_FULL))
		{
			hci_event_packet(hdev);;
		}
	}
	hci_create_connect( hdev, (char*)(hdev->hci_inquiry_info.bdaddr));
	__bt_print_to_sdc(hdev);
	__delay(5);
	while(hdev->link_flags)
	{
		while((hdev->port->rx_buffer_status==SDC_BUFFER_DATA)||(hdev->port->rx_buffer_status==SDC_BUFFER_FULL))
		{
			hci_event_packet(hdev);;
		}
	}
}
unsigned char acl_demo_data[15]={0x02,0x01,0x20,0x0A,0x00,0x06,0x00,0x01,0x00,0x48,0x45,0x4C,0x4C,0x4F,0x00};
void HCI_Send_acl_demo(struct hci_dev *hdev)
{
	int i;
	for(i=0;i<15;i++)
	{
		HCI_Put_char(hdev, *(acl_demo_data+i));
	}
	__bt_print_to_sdc(hdev);

}

void HCI_SCO_conn(struct hci_dev *hdev)
{
	hdev->link_flags=1;
	hci_add_sco_conn(hdev);
	while(!(hdev->link_flags))
	{
		while((hdev->port->rx_buffer_status==SDC_BUFFER_DATA)||(hdev->port->rx_buffer_status==SDC_BUFFER_FULL))
		{
			hci_event_packet(hdev);;
		}
	}
}

