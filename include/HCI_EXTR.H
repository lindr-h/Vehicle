#ifndef __HCI_EXTR_H
#define __HCI_EXTR_H
#include "HCI.H"
//INTERNAL FUNCTION
int __bt_mem_cpy(void * dst,void *src,int len);
int __bt_mem_cmp(void* buff1,void* buff2,int length);
void __bt_mem_set(void *buff,char ch,unsigned count);
UINT16 __host_to_hci_16(UINT16 i);
__inline int HCI_Put_char(struct hci_dev *hdev,char ch);
__inline unsigned char HCI_Get_char(struct hci_dev *hdev);
__inline void __bt_print_to_sdc(struct hci_dev *hdev);
void __delay(int counter);
__inline void hci_req_complete(struct hci_dev *hdev, int result);
__inline void hci_cmd_complete(struct hci_dev *hdev, int result);
//HCI ESSENTIAL OPERATION FUNCTION
void hci_send_cmd(struct hci_dev *hdev,UINT16 ogf,UINT8 ocf,UINT32 plen,void *param);
void hci_inq_req(struct hci_dev *hdev);
void hci_conn_add(struct hci_dev *hdev,UINT16 handle,UINT8 type,char *dst);
void hci_conn_del(struct hci_dev *hdev);
int hci_create_connect(struct hci_dev *hdev,char *bdaddr);
int hci_disconnect(struct hci_dev *hdev,UINT8 reason);
int hci_add_sco_conn(struct hci_dev *hdev,UINT16 pkt_type);
int hci_accept_conn_req(struct hci_dev *hdev);
int hci_reject_conn_req(struct hci_dev *hdev,UINT8 reason);
int hci_remote_name_req(struct hci_dev *hdev);
void hci_scan_req(struct hci_dev *hdev);
void hci_auth_req(struct hci_dev *hdev);
void hci_reset_req(struct hci_dev *hdev);

void hci_cc_link_policy(struct hci_dev *hdev, UINT16 ocf);
void hci_cc_link_ctl(struct hci_dev *hdev, UINT16 ocf);
void hci_cc_host_ctl(struct hci_dev *hdev,UINT16 ocf);
void hci_cc_info_param(struct hci_dev *hdev,UINT16 ocf);
void hci_cs_link_ctl(struct hci_dev *hdev,UINT16 ocf,UINT8 status);
void hci_cs_link_policy(struct hci_dev *hdev,UINT16 ocf,UINT8 status);
void hci_cs_host_ctl(struct hci_dev *hdev,UINT16 ocf,UINT8 status);
void hci_cs_info_param(struct hci_dev *hdev,UINT16 ocf,UINT8 status);
void hci_inquiry_complete_evt(struct hci_dev *hdev);
void hci_inquiry_result_evt(struct hci_dev *hdev);
void hci_conn_request_evt(struct hci_dev *hdev);
void hci_conn_complete_evt(struct hci_dev *hdev);
void hci_disconn_complete_evt(struct hci_dev *hdev);
void hci_num_comp_pkts_evt(struct hci_dev *hdev);
void hci_event_packet(struct hci_dev *hdev);

//HCI USER FUNCTION INTERFACE
void hci_init_req1(struct hci_dev *hdev);
void hci_init_req2(struct hci_dev *hdev);
void HCI_master_create_acl_conn(struct hci_dev *hdev);
void HCI_Send_acl_demo(struct hci_dev *hdev);
void HCI_SCO_conn(struct hci_dev *hdev);
#endif