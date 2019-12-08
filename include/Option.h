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
/*      option.h                                   S3c2410 USB device 1.0*/
/*                                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains  constants used in the S3c2410 USB protocol 	 */
/*                                                                       */
/* AUTHOR                                                                */
/*    	Zhi-gang yang   shenzhen watertek                                */
/*                                                                       */
/*                                                                       */
/* DATA STRUCTURES                                                       */
/*                                                                       */
/* 	None                                                             */
/*                                                                       */
/*                                                                       */
/* FUNCTIONS                                                             */
/*                                                                       */
/*      None                                                             */
/*                                                                       */
/* DEPENDENCIES                                                          */
/*                                                                       */
/*      2410addr.h	                   S3c2410 register address	 */
/*                                                                       */
/* HISTORY                                                               */
/*                                                                       */
/*         DATE                    REMARKS                               */
/*                                                                       */
/*      2004-12-31      Created initial version 1.0                      */
/*************************************************************************/
#ifndef __OPTION_H__
#define __OPTION_H__

//#define FCLK 135000000

#define FCLK 180000000
//#define FCLK 202800000
//#define FCLK 250000000
//#define FCLK 260000000
//#define FCLK 266000000
//#define FCLK 270000000
#define HCLK (FCLK/2)
#define PCLK (HCLK/2)
//#define PCLK (HCLK)
#define UCLK 48000000

// BUSWIDTH : 16,32
#define BUSWIDTH    (32)

//64MB
// 0x30000000 ~ 0x30ffffff : Download Area (16MB) Cacheable
// 0x31000000 ~ 0x33feffff : Non-Cacheable Area
// 0x33ff0000 ~ 0x33ff47ff : Heap & RW Area
// 0x33ff4800 ~ 0x33ff7fff : FIQ ~ User Stack Area
// 0x33ff8000 ~ 0x33fffeff : Not Useed Area
// 0x33ffff00 ~ 0x33ffffff : Exception & ISR Vector Table

#define _RAM_STARTADDRESS 	0x30000000
#define _ISR_STARTADDRESS 	0x33ffff00     
#define _MMUTT_STARTADDRESS	0x33ff8000
#define _STACK_BASEADDRESS	0x33ff8000
#define HEAPEND		  	0x33ff0000
#define _NONCACHE_STARTADDRESS	0x31000000

//If you use ADS1.x, please define ADS10
#define ADS10 TRUE

//USB Device Options
#define USBDMA		TRUE
#define USBDMA_DEMAND 	FALSE	//the downloadFileSize should be (64*n)
#define BULK_PKT_SIZE	32

// note: makefile,option.a should be changed

#endif /*__OPTION_H__*/
