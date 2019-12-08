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
/*      USB.h                                      S3c2410 USB device 1.0*/
/*                                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains data structure definitions and constants used */
/*      in the USB protocol 						 */
/*                                                                       */
/* AUTHOR                                                                */
/*    	Zhi-gang yang   shenzhen watertek                                */
/*                                                                       */
/*                                                                       */
/* DATA STRUCTURES                                                       */
/*                                                                       */
/*      USB_SETUP_DATA                        USB setup package          */
/*      USB_DEVICE_DESCRIPTOR                 USB device descriptor      */
/*      USB_CONFIGURATION_DESCRIPTOR          USB configuration descriptor */
/*      USB_INTERFACE_DESCRIPTOR              USB interface descriptor   */
/*      USB_ENDPOINT_DESCRIPTOR               USB EndPoint  descriptor   */
/*      USB_CONFIGURATION_SET                 USB configuration value    */
/*      USB_GET_STATUS                        USB status                 */
/*      USB_INTERFACE_GET                     USB interface value        */
/*                                                                       */
/* FUNCTIONS                                                             */
/*                                                                       */
/*      None                                                             */
/*                                                                       */
/* DEPENDENCIES                                                          */
/*                                                                       */
/*      None	                          				 */
/*                                                                       */
/* HISTORY                                                               */
/*                                                                       */
/*         DATE                    REMARKS                               */
/*                                                                       */
/*      2004-12-22      Created initial version 1.0                      */
/*************************************************************************/
#ifndef __USB_H__
#define __USB_H__

//************************
//       Endpoint 0      
//************************

// Standard bmRequestTyje (Direction) 
#define HOST_TO_DEVICE              (0x00)
#define DEVICE_TO_HOST              (0x80)    

// Standard bmRequestType (Type) 
#define STANDARD_TYPE               (0x00)
#define CLASS_TYPE                  (0x20)
#define VENDOR_TYPE                 (0x40)
#define RESERVED_TYPE               (0x60)

// Standard bmRequestType (Recipient) 
#define DEVICE_RECIPIENT            (0)
#define INTERFACE_RECIPIENT         (1)
#define ENDPOINT_RECIPIENT          (2)
#define OTHER_RECIPIENT             (3)

// Feature Selectors 
#define DEVICE_REMOTE_WAKEUP        (1)
#define EP_STALL                    (0)

// Standard Request Codes 
#define GET_STATUS                  (0)
#define CLEAR_FEATURE               (1)
#define SET_FEATURE                 (3)
#define SET_ADDRESS                 (5)
#define GET_DESCRIPTOR              (6)
#define SET_DESCRIPTOR              (7)
#define GET_CONFIGURATION           (8)
#define SET_CONFIGURATION           (9)
#define GET_INTERFACE               (10)
#define SET_INTERFACE               (11)
#define SYNCH_FRAME                 (12)

// Class-specific Request Codes 
#define GET_DEVICE_ID               (0)
#define GET_PORT_STATUS             (1)
#define SOFT_RESET                  (2)

// Descriptor Types
#define DEVICE_TYPE                 (1)
#define CONFIGURATION_TYPE          (2)
#define STRING_TYPE                 (3)
#define INTERFACE_TYPE              (4)
#define ENDPOINT_TYPE               (5)

//configuration descriptor: bmAttributes 
#define CONF_ATTR_DEFAULT	    (0x80) //Spec 1.0 it was BUSPOWERED bit.
#define CONF_ATTR_REMOTE_WAKEUP     (0x20) //remote wakeup	
#define CONF_ATTR_SELFPOWERED       (0x40) //self powered

//endpoint descriptor
#define EP_ADDR_IN		    (0x80) 	
#define EP_ADDR_OUT		    (0x00)

#define EP_ATTR_CONTROL		    (0x0)  //control transfers	
#define EP_ATTR_ISOCHRONOUS	    (0x1)  //Isochronous transfers
#define EP_ATTR_BULK		    (0x2)  //Bulk transfers
#define EP_ATTR_INTERRUPT	    (0x3)  //Interrupt transfers	


//string descriptor
#define LANGID_US_L 		    (0x09)  
#define LANGID_US_H 		    (0x04)

//USB setup package
struct USB_SETUP_DATA{
    U8 bmRequestType;    	//request type
    U8 bRequest;         	//Standard Request
    U8 bValueL;          	//value 
    U8 bValueH;          	//value
    U8 bIndexL;        		//request index  
    U8 bIndexH;          	//request index
    U8 bLengthL;         	//length
    U8 bLengthH;         	//length
};

//USB device descriptor
struct USB_DEVICE_DESCRIPTOR{
    U8 bLength;    		//length=0x12
    U8 bDescriptorType;         //device descriptor type =0x01
    U8 bcdUSBL;			//BCD
    U8 bcdUSBH;			//BCD
    U8 bDeviceClass;          	//type code 
    U8 bDeviceSubClass;         //device subclass 
    U8 bDeviceProtocol;         //Protocol code
    U8 bMaxPacketSize0;         //ep0 max packet size ,only 8,16,32,64 avail
    U8 idVendorL;		//Vendor
    U8 idVendorH;		//Vendor
    U8 idProductL;		//product id
    U8 idProductH;		//product id
    U8 bcdDeviceL;		//product 
    U8 bcdDeviceH;		//product
    U8 iManufacturer;		//vendor descriptor index
    U8 iProduct;		//product descriptor string index
    U8 iSerialNumber;		//serial number string index
    U8 bNumConfigurations;	//configuration number
};


struct USB_CONFIGURATION_DESCRIPTOR{
    U8 bLength;    		//configuration descriptor size =0x9
    U8 bDescriptorType;  	//configuration descriptor type =0x2       
    U8 wTotalLengthL;		//total length,include all descriptor,such as configuration
    				//.interface.endpoint.string,in this configuration
    U8 wTotalLengthH;
    U8 bNumInterfaces;		//interface number in this configuration
    U8 bConfigurationValue;	//select configuration value as parameter  of "set configuration"
    U8 iConfiguration;		//string descriptor index
    U8 bmAttributes;		//configurator attribute   D7:bus power  D6: self power D5:remote wakeup D4..0:reserve 
    U8 maxPower;          	//electricity :2mA*n
};
    

struct USB_INTERFACE_DESCRIPTOR{
    U8 bLength;    		//interface descriptor size =0x9
    U8 bDescriptorType;         //interface descriptor type =0x4
    U8 bInterfaceNumber;	//interface descriptor serial
    U8 bAlternateSetting;	//用于为上一字段选择可供替换的设置
    U8 bNumEndpoints;		//number of endpoint (exclude endpoint0)
    U8 bInterfaceClass;		//interface class
    U8 bInterfaceSubClass;	//interface subclass
    U8 bInterfaceProtocol;	//protocol code
    U8 iInterface;		//string descriptor index
};


struct USB_ENDPOINT_DESCRIPTOR{
    U8 bLength;    		//endpoint descriptor size=0x7
    U8 bDescriptorType;         //interface descriptor type =0x5
    U8 bEndpointAddress;	//endpoint address D0..3:endpoint value D4..6:reserve D7:direction 0:out 1:in
    U8 bmAttributes;		//endpoint attribute,tranfer type
    U8 wMaxPacketSizeL;		//max packet size
    U8 wMaxPacketSizeH;
    U8 bInterval;		//time slot 
};

 struct USB_CONFIGURATION_SET{
     U8 ConfigurationValue;	//save configuration value for set configuration request
 };

 struct USB_GET_STATUS{
     U8 Device;			//device status
     U8 Interface;		//interface status: 16 byte of interface status reserve, it is oK to return 0
     U8 Endpoint0;		//ep0 status: =0 resume =1 disable
     U8 Endpoint1;		//ep1 status: =0 resume =1 disable
     U8 Endpoint3;		//ep3 status: =0 resume =1 disable
 };

 struct USB_INTERFACE_GET{
     U8 AlternateSetting;	//save for interface configuration
 };


 
#endif /*__USB_H__*/                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
