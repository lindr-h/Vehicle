/*************************************************************************/
/*                                                                       */
/*               Copyright Shenzhen Watertek S.&T. Co.,Ltd  2002         */
/*                         All Rights Reserved.                          */
/*                                                                       */
/* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS  */
/* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS   */
/* SUBJECT TO LICENSE TERMS.                                             */
/*                                                                       */
/*************************************************************************/

/*************************************************************************/
/*                                                                       */
/* FILE NAME                                               VERSION       */
/*                                                                       */
/*      usbsetup.c                                 S3c2410 USB device 1.0*/
/*                                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains data structure definitions , constants 	 */
/*	and manipulation facilitiesused  in 2410 ep0(setup) 		 */     
/* AUTHOR                                                                */
/*    	Zhi-gang yang   shenzhen watertek                                */
/*                                                                       */
/* DATA STRUCTURES                                                       */
/*      None                                                                 */
/*                                                                       */
/* FUNCTIONS                                                             */
/*                                                                       */
/*      PrepareEp1Fifo                        prepare ep1 FIFO and setup */
/*					      ep1 already, if token IN 	 */
/*					      arrive, data send to host	 */
/*      Ep1Handler                            ep1 interrupt handler	 */
/*      PrintEpiPkt                           print ep1 packet,only for  */
/*						debug 			 */
/*                                                                       */
/* DEPENDENCIES                                                          */
/*      usbmain.h                                                        */
/*      usb.h                                                            */
/*      usblib.h                                                         */
/*      usbsetup.h                                                       */
/*      option.h	                       				 */
/*      string.h	                       				 */
/*      2410addr.h                          				 */
/*      2410usb.h                                                        */
/*      2410lib.h                         				 */
/*      def.h                                                            */
/* HISTORY                                                               */
/*                                                                       */
/*         DATE                    REMARKS                               */
/*                                                                       */
/*      2005-1-8       Created initial version 1.0                       */
/*      2005-1-27       modified by yangzg	                         */
/*************************************************************************/

#include <string.h>
#include "option.h"
#include "2410addr.h"
#include "def.h"
#include "2410usb.h"
#include "usbmain.h"
#include "usb.h"
#include "usblib.h"
#include "usbsetup.h"
//extern volatile int isUsbdSetConfiguration;
// *** End point information ***
//   EP0: control
//   EP1: bulk in end point
//   EP2: not used
//   EP3: bulk out end point
//   EP4: not used

// *** VERY IMPORTANT NOTE ***
// Every descriptor size of EP0 should be 8n+m(m=1~7).
// Otherwise, USB will not operate normally because the program
// doesn't prepare the case that the descriptor size is 8n+0.
// If the size of a descriptor is 8n, the 0 length packit should be sent. 
// Special thanks to E.S.Choi for reminding me of this USB specification.


// ===================================================================
// All following commands will operate only in case 
// - ep0_csr is valid.
// ===================================================================
//clear ep0 out packet ready
#define CLR_EP0_OUT_PKT_RDY() 		rEP0_CSR=( ep0_csr & (~EP0_WR_BITS)| \
						EP0_SERVICED_OUT_PKT_RDY )
//clear ep0 packet 0	 
#define CLR_EP0_OUTPKTRDY_DATAEND() 	rEP0_CSR=( ep0_csr & (~EP0_WR_BITS)| \
						(EP0_SERVICED_OUT_PKT_RDY|EP0_DATA_END) )	 
//set ep0 IN packet ready					
#define SET_EP0_IN_PKT_RDY() 		rEP0_CSR=( ep0_csr & (~EP0_WR_BITS)| \
						(EP0_IN_PKT_READY) )
//send packet 0	 
#define SET_EP0_INPKTRDY_DATAEND() 	rEP0_CSR=( ep0_csr & (~EP0_WR_BITS)| \
						(EP0_IN_PKT_READY|EP0_DATA_END) )	 
//clear ep0 setup end tag					
#define CLR_EP0_SETUP_END() 		rEP0_CSR=( ep0_csr & (~EP0_WR_BITS)| \
						(EP0_SERVICED_SETUP_END) )
//clear ep0 sent stall tag(host ---->mcu)
#define CLR_EP0_SENT_STALL() 		rEP0_CSR=( ep0_csr & (~EP0_WR_BITS)& \
						(~EP0_SENT_STALL) )
//flush ep0 fifo
//#define FLUSH_EP0_FIFO() 		{while(rOUT_FIFO_CNT1_REG)rEP0_FIFO;}
#define FLUSH_EP0_FIFO() 		{while(rOUT_FIFO_CNT1_REG);}

U32 ep0State;
U32 ep0SubState;


volatile U8 Rwuen;
volatile U8 Configuration=1;
volatile U8 AlterSetting;
volatile U8 Selfpwr=TRUE;
volatile U8 device_status;
volatile U8 interface_status;
volatile U8 endpoint0_status;
volatile U8 endpoint1_status;
volatile U8 endpoint3_status;

struct USB_SETUP_DATA descSetup;
struct USB_DEVICE_DESCRIPTOR descDev;
struct USB_CONFIGURATION_DESCRIPTOR descConf;
struct USB_INTERFACE_DESCRIPTOR descIf;
struct USB_ENDPOINT_DESCRIPTOR descEndpt0;
struct USB_ENDPOINT_DESCRIPTOR descEndpt1;
struct USB_CONFIGURATION_SET ConfigSet;
struct USB_INTERFACE_GET InterfaceGet;
struct USB_GET_STATUS StatusGet;   //={0,0,0,0,0};


static const U8 descStr0[]={
	4,STRING_TYPE,LANGID_US_L,LANGID_US_H,  //codes representing languages
    };

static const U8 descStr1[]={  //Manufacturer  
        (0x14+2),STRING_TYPE, 
        'S',0x0,'y',0x0,'s',0x0,'t',0x0,'e',0x0,'m',0x0,' ',0x0,'M',0x0,
        'C',0x0,'U',0x0,
    };
    
static const U8 descStr2[]={  //Product  
        (0x2a+2),STRING_TYPE, 
        'S',0x0,'E',0x0,'C',0x0,' ',0x0,'S',0x0,'3',0x0,'C',0x0,'2',0x0,
        '4',0x0,'1',0x0,'0',0x0,'X',0x0,' ',0x0,'T',0x0,'e',0x0,'s',0x0,
        't',0x0,' ',0x0,'B',0x0,'/',0x0,'D',0x0
    };

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Ep0Handler		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      Ep0 interrupt handler				                 */
/*                                                                       */
/* AUTHOR                                                                */
/*                                                                       */
/*      Zhi-gang Yang          shenzhen watertek                         */
/*                                                                       */
/* INPUTS                                                                */
/*      None                                                             */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None      	                                                 */
/*                                                                       */
/* HISTORY                                                               */
/*                                                                       */
/*         DATE                    REMARKS                               */
/*                                                                       */
/*      2004-12-28      Created initial version 1.0                      */
/*************************************************************************/

void Ep0Handler(void)
{
    static int ep0SubState;
    U8 ep0_csr;

    rINDEX_REG=0;
    ep0_csr=rEP0_CSR;
    

    //DATAEND interrupt(ep0_csr==0x0) will be ignored 
    //because ep0State==EP0_STATE_INIT when the DATAEND interrupt is issued.

    
    if(ep0_csr & EP0_SETUP_END)					//setup_end is set
    {   
    	 // Host may end GET_DESCRIPTOR operation without completing the IN data stage.
    	 // If host does that, SETUP_END bit will be set.
    	 // OUT_PKT_RDY has to be also cleared because status stage sets OUT_PKT_RDY to 1.
	CLR_EP0_SETUP_END();
	if(ep0_csr & EP0_OUT_PKT_READY) 			//OUT_PKT_RDY is set
	{
	    FLUSH_EP0_FIFO(); 
	    CLR_EP0_OUT_PKT_RDY();
	}
	
	ep0State=EP0_STATE_INIT;
	return;
    }	

    if(ep0_csr & EP0_SENT_STALL)				//SENT_STALL is set
    {   
   	CLR_EP0_SENT_STALL();
	if(ep0_csr & EP0_OUT_PKT_READY) 
	{
	    CLR_EP0_OUT_PKT_RDY();
	}
	
	ep0State=EP0_STATE_INIT;
	return;
    }	



    if((ep0_csr & EP0_OUT_PKT_READY) && (ep0State==EP0_STATE_INIT))	//deal with setup packet
    {	
	RdPktEp0((U8 *)&descSetup,EP0_PKT_SIZE);

	switch(descSetup.bRequest)				
    	{
    	case GET_DESCRIPTOR:				//handle GET_DESCRIPTOR request
            switch(descSetup.bValueH)        
            {
            case DEVICE_TYPE:				//device descriptor
 	 	    	CLR_EP0_OUT_PKT_RDY();
	    	ep0State=EP0_STATE_GD_DEV_0;	        
	    	break;	
	    case CONFIGURATION_TYPE:			//configuration descriptor
 	 	    	CLR_EP0_OUT_PKT_RDY();
 	    	if((descSetup.bLengthL+(descSetup.bLengthH<<8))>0x9)
 	    	  //bLengthH should be used for bLength=0x209 at WIN2K.    	
	    	   ep0State=EP0_STATE_GD_CFG_0; //for WIN98,WIN2K
               else	    	    
  		    ep0State=EP0_STATE_GD_CFG_ONLY_0; //for WIN2K
	    	break;
   	    case STRING_TYPE:				//string descriptor
 	 	    	CLR_EP0_OUT_PKT_RDY();
	    	switch(descSetup.bValueL)
	    	{
	    	    case 0:
	    	    	ep0State=EP0_STATE_GD_STR_I0;
	    	    	break;
	    	    case 1:
       	    	    	ep0State=EP0_STATE_GD_STR_I1;
	    	    	break;
	    	    case 2:	
	    	    	ep0State=EP0_STATE_GD_STR_I2;
	    	    	break;
	    	    default:
		    		break;
	    	}
	    	ep0SubState=0;
	    	break;
	    case INTERFACE_TYPE:			//interface descriptor
 	 	    	CLR_EP0_OUT_PKT_RDY();
	    	ep0State=EP0_STATE_GD_IF_ONLY_0; //for WIN98
	    	break;
	    case ENDPOINT_TYPE:	    			//endpoint descriptor
 	    	CLR_EP0_OUT_PKT_RDY();
 	    	switch(descSetup.bValueL&0xf)
	    	{
	    	case 0:
	    	    ep0State=EP0_STATE_GD_EP0_ONLY_0;
	    	    break;
	    	case 1:
       	    	    ep0State=EP0_STATE_GD_EP1_ONLY_0;
	    	    break;
	    	default:
	    	    break;
	    	}
	    	break;
	    default:
	    	break;
	    }	
    	    break;

    	case SET_ADDRESS:				//set address request
            rFUNC_ADDR_REG=descSetup.bValueL | 0x80;
	    CLR_EP0_OUTPKTRDY_DATAEND(); //Because of no data control transfers.
            ep0State=EP0_STATE_INIT;
            break;
    	
	case SET_CONFIGURATION:				//set configuration request
            ConfigSet.ConfigurationValue=descSetup.bValueL;
            CLR_EP0_OUTPKTRDY_DATAEND(); //Because of no data control transfers.
            ep0State=EP0_STATE_INIT;

            //isUsbdSetConfiguration=1; 
            break;

    	    //////////////////////// For chapter 9 test ////////////////////

    	  case CLEAR_FEATURE:				//clear  configuration request
    	  	
    	  	switch (descSetup.bmRequestType)
    	  	{
    	  	  case DEVICE_RECIPIENT:		//device speciality 
    	  	  	if (descSetup.bValueL == 1)
    	  	  		Rwuen = FALSE;
    	  	  	
    	  	  	break;

    	  	  case ENDPOINT_RECIPIENT:		//endpoint speciality
    	  	  	 if (descSetup.bValueL == 0)
    	  	  	 {
	                       if((descSetup.bIndexL & 0x7f) == 0x00){
	                        StatusGet.Endpoint0= 0;    
	                       }
	                       if((descSetup.bIndexL & 0x8f) == 0x81){           // IN  Endpoint 1
	                         StatusGet.Endpoint1= 0;           
	                        }
	                       if((descSetup.bIndexL & 0x8f) == 0x03){          // OUT Endpoint 3
	                         StatusGet.Endpoint3= 0;      
	                         }
                       }
    	  	  	 
    	  	  	 break;

    	  	  default:
    	  	  	break;
    	  	}
    	  	CLR_EP0_OUTPKTRDY_DATAEND();
    	  	ep0State=EP0_STATE_INIT;
    	  	break;

    	  case GET_CONFIGURATION:			//Get configuration request

                CLR_EP0_OUT_PKT_RDY();
	    	  ep0State=EP0_CONFIG_SET;
    	  	   
    	         break;


    	  case GET_INTERFACE:				//Get interface request
    	  	
    	  	  CLR_EP0_OUT_PKT_RDY();
	    	  ep0State=EP0_INTERFACE_GET;
    	  	  
    	  	  break;

    	  case GET_STATUS:				//Get status

    	  	switch(descSetup.bmRequestType)
    	  	{
    	  	      case  (0x80):			//get device status

    	  	 		CLR_EP0_OUT_PKT_RDY();
    	  	 		StatusGet.Device=((U8)Rwuen<<1)|(U8)Selfpwr;
    	  	 		ep0State=EP0_GET_STATUS0;
    	  	 		    	  	 		
                          break;

                     case  (0x81):			//get interface status
                           	CLR_EP0_OUT_PKT_RDY();
    	  	 		StatusGet.Interface=0;
    	  	 		ep0State=EP0_GET_STATUS1;
                          break;

                     case  (0x82):			//get endpoint

                     	CLR_EP0_OUT_PKT_RDY();
    	  	 		if((descSetup.bIndexL & 0x7f) == 0x00){			//get ep0 status
	                          ep0State=EP0_GET_STATUS2;
    	  	 		  }
    	  	 		
	                       if((descSetup.bIndexL & 0x8f) == 0x81){			//get ep1 status
	                          ep0State=EP0_GET_STATUS3;
	                       }
	                       
	                       if((descSetup.bIndexL & 0x8f) == 0x03){			//get ep3 status
                                ep0State=EP0_GET_STATUS4;
	                       }
                          break;

                      default:
                          	break;
    	  	}
    	  	      
    	  	break;


    	  case SET_DESCRIPTOR:						//set descriptor request
    	  	       CLR_EP0_OUTPKTRDY_DATAEND();
    	  	       ep0State=EP0_STATE_INIT;
    	  	break;


    	  case SET_FEATURE:						//set feature request
    	  	 
    	  	switch (descSetup.bmRequestType)
    	  	{
    	  	  case DEVICE_RECIPIENT:				//set device recipient
    	  	  	if (descSetup.bValueL == 1)
    	  	  		Rwuen = TRUE;
    	  	  	
    	  	  	break;

    	  	  case ENDPOINT_RECIPIENT:				//set endpoint recipient
    	  	  	 if (descSetup.bValueL == 0)
    	  	  	 {
	                       if((descSetup.bIndexL & 0x7f) == 0x00){
	                        StatusGet.Endpoint0= 1;
	                       }
	                       if((descSetup.bIndexL & 0x8f) == 0x81){
	                         StatusGet.Endpoint1= 1;
	                       }
	                       if((descSetup.bIndexL & 0x8f) == 0x03){
	                         StatusGet.Endpoint3= 1;
	                       }
                       }
    	  	  	break;

    	  	  default:
    	  	  	break;
    	  	}
    	  	CLR_EP0_OUTPKTRDY_DATAEND();
    	  	ep0State=EP0_STATE_INIT;
    	  
    	  	break;


    	  case SET_INTERFACE:						//set interface request
    	  	   InterfaceGet.AlternateSetting= descSetup.bValueL;
    	  	   CLR_EP0_OUTPKTRDY_DATAEND(); 
                 ep0State=EP0_STATE_INIT;
    	  	break;

    	  case SYNCH_FRAME:						//no use in this case
    	  	ep0State=EP0_STATE_INIT;
    	  	break;

    	  //////////////////////////////////////////////////////////////

  	default:
    	    CLR_EP0_OUTPKTRDY_DATAEND(); //Because of no data control transfers.
	    ep0State=EP0_STATE_INIT;
	    break;
    
        }
    }
    
    switch(ep0State)
    {	
	case EP0_STATE_INIT:
	    break; 

	//=== GET_DESCRIPTOR:DEVICE ===
    	case EP0_STATE_GD_DEV_0:				//send device descriptor to host by three times
             WrPktEp0((U8 *)&descDev+0,8); //EP0_PKT_SIZE
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_DEV_1;
            break;
            
    	case EP0_STATE_GD_DEV_1:
            WrPktEp0((U8 *)&descDev+0x8,8); 
            SET_EP0_IN_PKT_RDY();
            
            ep0State=EP0_STATE_GD_DEV_2;
            break;

    	case EP0_STATE_GD_DEV_2:
             WrPktEp0((U8 *)&descDev+0x10,2);   //8+8+2=0x12
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;
            break;
    

        //=== GET_DESCRIPTOR:CONFIGURATION+INTERFACE+ENDPOINT0+ENDPOINT1 ===
        //Windows98 gets these 4 descriptors all together by issuing only a request.
        //Windows2000 gets each descriptor seperately.
    	case EP0_STATE_GD_CFG_0:
            WrPktEp0((U8 *)&descConf+0,8); //EP0_PKT_SIZE
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_1;
            break;
    
    	case EP0_STATE_GD_CFG_1:
             WrPktEp0((U8 *)&descConf+8,1); 
            WrPktEp0((U8 *)&descIf+0,7); 
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_2;
            break;

    	case EP0_STATE_GD_CFG_2:
            WrPktEp0((U8 *)&descIf+7,2); 
            WrPktEp0((U8 *)&descEndpt0+0,6); 
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_3;
            break;

    	case EP0_STATE_GD_CFG_3:
             WrPktEp0((U8 *)&descEndpt0+6,1); 
            WrPktEp0((U8 *)&descEndpt1+0,7); 
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_4;            
            break;

    	case EP0_STATE_GD_CFG_4:
             //zero length data packit 
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;            
            break;

        //=== GET_DESCRIPTOR:CONFIGURATION ONLY===
    	case EP0_STATE_GD_CFG_ONLY_0:
               WrPktEp0((U8 *)&descConf+0,8); //EP0_PKT_SIZE
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_ONLY_1;
            break;
    
    	case EP0_STATE_GD_CFG_ONLY_1:
             WrPktEp0((U8 *)&descConf+8,1); 
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;            
            break;

        //=== GET_DESCRIPTOR:INTERFACE ONLY===
    	case EP0_STATE_GD_IF_ONLY_0:
             WrPktEp0((U8 *)&descIf+0,8); 
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_IF_ONLY_1;
            break;
    	case EP0_STATE_GD_IF_ONLY_1:
               WrPktEp0((U8 *)&descIf+8,1); 
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;            
            break;

        //=== GET_DESCRIPTOR:ENDPOINT 0 ONLY===
    	case EP0_STATE_GD_EP0_ONLY_0:
               WrPktEp0((U8 *)&descEndpt0+0,7); 
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;            
            break;
            
        //=== GET_DESCRIPTOR:ENDPOINT 1 ONLY===
    	case EP0_STATE_GD_EP1_ONLY_0:
               WrPktEp0((U8 *)&descEndpt1+0,7); 
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;            
            break;
            
////////////////////////////////////////////

         case EP0_INTERFACE_GET:					//send interface descriptor to host
            WrPktEp0((U8 *)&InterfaceGet+0,1);
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;      
         break;

 
        //=== GET_DESCRIPTOR:STRING ===

    	case EP0_STATE_GD_STR_I0:					//send string0 descriptor to host
             WrPktEp0((U8 *)descStr0, 4 );  
	    SET_EP0_INPKTRDY_DATAEND();
	    ep0State=EP0_STATE_INIT;     
	    ep0SubState=0;
	    break;

	case EP0_STATE_GD_STR_I1:					//send string1 descriptor to host
              if( (ep0SubState*EP0_PKT_SIZE+EP0_PKT_SIZE)<sizeof(descStr1) )
            {
            	WrPktEp0((U8 *)descStr1+(ep0SubState*EP0_PKT_SIZE),EP0_PKT_SIZE); 
            	SET_EP0_IN_PKT_RDY();
            	ep0State=EP0_STATE_GD_STR_I1;
            	ep0SubState++;
            }
	    else
	    {
	    	WrPktEp0((U8 *)descStr1+(ep0SubState*EP0_PKT_SIZE),
	    		 sizeof(descStr1)-(ep0SubState*EP0_PKT_SIZE)); 
		SET_EP0_INPKTRDY_DATAEND();
		ep0State=EP0_STATE_INIT;     
		ep0SubState=0;
	    }
	    break;

	case EP0_STATE_GD_STR_I2:					//send string2 descriptor to host
               if( (ep0SubState*EP0_PKT_SIZE+EP0_PKT_SIZE)<sizeof(descStr2) )
            {
            	WrPktEp0((U8 *)descStr2+(ep0SubState*EP0_PKT_SIZE),EP0_PKT_SIZE); 
            	SET_EP0_IN_PKT_RDY();
            	ep0State=EP0_STATE_GD_STR_I2;
            	ep0SubState++;
            }
	    else
	    {
  	    	WrPktEp0((U8 *)descStr2+(ep0SubState*EP0_PKT_SIZE),
	    		 sizeof(descStr2)-(ep0SubState*EP0_PKT_SIZE)); 
		SET_EP0_INPKTRDY_DATAEND();
		ep0State=EP0_STATE_INIT;     
		ep0SubState=0;
	    }
	    break;

	 case EP0_CONFIG_SET:						//set config DESCRIPTOR 
	 	WrPktEp0((U8 *)&ConfigSet+0,1); 
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;      
            break;

        case EP0_GET_STATUS0:						//send device status to host
	     WrPktEp0((U8 *)&StatusGet+0,1);
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;      
         break;

         case EP0_GET_STATUS1:						//send config status
	     WrPktEp0((U8 *)&StatusGet+1,1);
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;      
         break;

         case EP0_GET_STATUS2:						//send ep0 status
	     WrPktEp0((U8 *)&StatusGet+2,1);
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;      
         break;

         case EP0_GET_STATUS3:						//send ep1 status
	     WrPktEp0((U8 *)&StatusGet+3,1);
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;      
         break;

         case EP0_GET_STATUS4:						//send ep3 status
	     WrPktEp0((U8 *)&StatusGet+4,1);
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;      
         break;

         
	 	
     	default:
     	    break;
    }
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      InitDescriptorTable	                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      Init descriptor table				                 */
/*                                                                       */
/* AUTHOR                                                                */
/*                                                                       */
/*      Zhi-gang Yang          shenzhen watertek                         */
/*                                                                       */
/* INPUTS                                                                */
/*      None                                                             */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None      	                                                 */
/*                                                                       */
/* HISTORY                                                               */
/*                                                                       */
/*         DATE                    REMARKS                               */
/*                                                                       */
/*      2004-12-28      Created initial version 1.0                      */
/*************************************************************************/


void InitDescriptorTable(void)
{	
    //Standard device descriptor
    descDev.bLength=0x12;	//EP0_DEV_DESC_SIZE=0x12 bytes    
    descDev.bDescriptorType=DEVICE_TYPE;         
    descDev.bcdUSBL=0x10;
    descDev.bcdUSBH=0x01; 	//Ver 1.10
    descDev.bDeviceClass=0xFF; //0x0          
    descDev.bDeviceSubClass=0x0;          
    descDev.bDeviceProtocol=0x0;          
    descDev.bMaxPacketSize0=0x8;         
    descDev.idVendorL=0x45;
    descDev.idVendorH=0x53;
    descDev.idProductL=0x34;
    descDev.idProductH=0x12;
    descDev.bcdDeviceL=0x00;
    descDev.bcdDeviceH=0x01;
    descDev.iManufacturer=0x1;  //index of string descriptor
    descDev.iProduct=0x2;	//index of string descriptor 
    descDev.iSerialNumber=0x0;
    descDev.bNumConfigurations=0x1;

    //Standard configuration descriptor
    descConf.bLength=0x9;    
    descConf.bDescriptorType=CONFIGURATION_TYPE;         
    descConf.wTotalLengthL=0x20; //<cfg desc>+<if desc>+<endp0 desc>+<endp1 desc>
    descConf.wTotalLengthH=0;
    descConf.bNumInterfaces=1;
//dbg    descConf.bConfigurationValue=2;  //why 2? There's no reason.
    descConf.bConfigurationValue=1;  
    descConf.iConfiguration=0;
    descConf.bmAttributes=CONF_ATTR_DEFAULT|CONF_ATTR_SELFPOWERED;  //bus powered only.
    descConf.maxPower=25; //draws 50mA current from the USB bus.          

    //Standard interface descriptor
    descIf.bLength=0x9;    
    descIf.bDescriptorType=INTERFACE_TYPE;         
    descIf.bInterfaceNumber=0x0;
    descIf.bAlternateSetting=0x0; //?
    descIf.bNumEndpoints=2;	//# of endpoints except EP0
    descIf.bInterfaceClass=0xff; //0x0 ?
    descIf.bInterfaceSubClass=0x0;  
    descIf.bInterfaceProtocol=0x0;
    descIf.iInterface=0x0;

    //Standard endpoint0 descriptor
    descEndpt0.bLength=0x7;    
    descEndpt0.bDescriptorType=ENDPOINT_TYPE;         
    descEndpt0.bEndpointAddress=1|EP_ADDR_IN;   // 2400Xendpoint 1 is IN endpoint.
    descEndpt0.bmAttributes=EP_ATTR_BULK;
    descEndpt0.wMaxPacketSizeL=EP1_PKT_SIZE; //64
    descEndpt0.wMaxPacketSizeH=0x0;
    descEndpt0.bInterval=0x0; //not used

    //Standard endpoint1 descriptor
    descEndpt1.bLength=0x7;    
    descEndpt1.bDescriptorType=ENDPOINT_TYPE;         
    descEndpt1.bEndpointAddress=3|EP_ADDR_OUT;   // 2400X endpoint 3 is OUT endpoint.
    descEndpt1.bmAttributes=EP_ATTR_BULK;
    descEndpt1.wMaxPacketSizeL=EP3_PKT_SIZE; //64
    descEndpt1.wMaxPacketSizeH=0x0;
    descEndpt1.bInterval=0x0; //not used 
}

