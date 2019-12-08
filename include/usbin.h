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
/*      usbin.h                                 S3c2410 USB device 1.0	 */
/*                                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains  function  used  in the USB protocol          */
/* AUTHOR                                                                */
/*    	Zhi-gang yang   shenzhen watertek                                */
/*                                                                       */
/*                                                                       */
/* DATA STRUCTURES                                                       */
/*                                                                       */
/*  None                                                                 */
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

#ifndef __USBIN_H__
#define __USBIN_H__

void Ep1Handler(void);
void PrepareEp1Fifo(void);

#endif /*__USBIN_H__*/
