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
/*      usblib.h                                      S3c2410 USB device 1.0*/
/*                                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains S3c2410 lib					 */

/* AUTHOR                                                                */
/*    	Zhi-gang yang   shenzhen watertek                                */
/*                                                                       */
/*                                                                       */
/* DATA STRUCTURES                                                       */
/*                                                                       */
/*    none                                                               */
/* FUNCTIONS                                                             */
/*       ConfigUsbd           		config usb		         */
/*       ReconfigUsbd          		reconfig usb		         */
/*       RdPktEp0       		read Ep0 packet			 */
/*       WrPktEp0                       write packet into Ep0 FIFO       */
/*       WrByteEp0                      write byte into   Ep0 FIFO       */
/*       WrPktEp1                       write packet into Ep1 FIFO       */
/*       WrPktEp2                       write packet into Ep2 FIFO       */
/*       RdPktEp3                       read packet from Ep3 FIFO        */
/*       RdPktEp4                       read packet from Ep4 FIFO        */
/*       ConfigEp3IntMode               config Ep3 interrupt mode        */
/*       ConfigEp3DmaMode               config Ep3 DMA mode              */
/*                                                                       */
/*                                                                       */
/* DEPENDENCIES                                                          */
/*       2410usb.h                                                                */
/*      	                          				 */
/*                                                                       */
/* HISTORY                                                               */
/*                                                                       */
/*         DATE                    REMARKS                               */
/*                                                                       */
/*      2004-12-22      Created initial version 1.0                      */
/*************************************************************************/
#ifndef __USBLIB_H__
#define __USBLIB_H__

#include "2410usb.h"

#define PWR_REG_DEFAULT_VALUE (DISABLE_SUSPEND)

void ConfigUsbd(void);
void ReconfigUsbd(void);

void RdPktEp0(U8 *buf,int num);
void WrPktEp0(U8 *buf,int num);
void WrByteEp0(U8 value);
void WrPktEp1(U8 *buf,int num);
void WrPktEp2(U8 *buf,int num);
void RdPktEp3(U8 *buf,int num);
void RdPktEp4(U8 *buf,int num);

void ConfigEp3IntMode(void);
void ConfigEp3DmaMode(U32 bufAddr,U32 count);

#endif /*__USBLIB_H__*/

