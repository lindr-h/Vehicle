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
/*      usbout.h                                      S3c2410 USB device 1.0*/
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
/*    none                                                               */
/* FUNCTIONS                                                             */
/*       Ep3Handler           		Ep3 interrupt handler            */
/*       IsrDma2             		Dma2 interrupt handler           */
/*       ClearEp3OutPktReady            clear Ep3 out packet read tag 	 */
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
#ifndef __USBOUT_H__
#define __USBOUT_H__

void Ep3Handler(void);
void IsrDma2(void);
void ClearEp3OutPktReady(void);

#endif /*__USBOUT_H__*/
