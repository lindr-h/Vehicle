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
/*      def.h                                  S3c2410 USB device 1.0	 */
/*                                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains  constants for data type		 	 */
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
/*      None								 */
/*                                                                       */
/* HISTORY                                                               */
/*                                                                       */
/*         DATE                    REMARKS                               */
/*                                                                       */
/*      2004-12-31      Created initial version 1.0                      */
/*************************************************************************/
#ifndef __DEF_H__
#define __DEF_H__



#define U32			unsigned int
#define U16			unsigned short
#define S32			int
#define S16			short int
#define U8			unsigned char
#define	S8			char
#define BPP16		(16)
#define BYTE_BIT	(8)
#define TRUE		1   
#define FALSE		0
#define False		0
#define True		1

#ifdef STN
#define VideoAddrStart	(0x30000000)
#define VideoAddrEnd	((0x30000000+640*480*12*2/8))
#define x_size (480)
#define X_MAX (640)
#define Y_MAX (480)
#else
#define VideoAddrStart	(0x30000000)
#define VideoAddrEnd	((0x30000000+240*320*16/8))
//#define x_size (240)

//#ifdef TFT320X240
/*=======TFT 3.4´çÕæ²ÊÆÁ(320*240)=======*/
//#define X_MAX (320)
//#define Y_MAX (240)
//#else
/*=======TFT 3.4´çÕæ²ÊÆÁ(240*320)=======*/
//#define X_MAX (240)
//#define Y_MAX (320)
//#endif

extern int mGuiOrientation;
extern int g_xMax;
extern int g_yMax;

#define SUB_RUN			1
#define SUB_STOP		0

#endif

#define COLOR int

//×Ö¿â´æ·ÅµÄµØÖ·
#define hzbufbase		0x140000		//the chinse character base address which was in the scope of flash
#define asciibufbase	0x120000		//the ASCII base adddress which was in the scope of flash
#define IMAGEBUFFER		0x60000			//image base address
#define WavBaseAddr		0xa0000
#define IMAGEBUFFER2	0x1c0000		//image base address


//color
#define RED_BITNUM		5               //the bit number of the red color in red-blue-green
#define RED_SHIFT		11              //the shift number of the red color from the right
#define GREEN_BITNUM	6               //the bit number of the green color in red-blue-green
#define GREEN_SHIFT		5               //the shift number of the green color from the right
#define BLUE_BITNUM		5               //the bit number of the blue color in red-blue-green
#define BLUE_SHIFT		0               //the shift number of the blue color from the right



/*the following defined the touchpanel register address*/
#define ADCCON			0X58000000
#define ADCTSC			0X58000004
#define ADCDLY			0X58000008
#define ADCDAT0			0X5800000C
#define ADCDAT1			0X58000010

#define TP_XRAGE		((float)319/650)	//the convision's coefficient of touchpanel
#define	TP_YRAGE		((float)239/752)	//the convision's coefficient of touchpanel


#define TP_XRAGE2		0X388				//the convision's coefficient of touchpanel
#define	TP_YRAGE2		0X388				//the convision's coefficient of touchpanel

#define TP_YBASE		85
#define TP_XBASE		858


#define LINK_HWORD(c1,c2)		((U16)c1<<8) | c2
#define LINK_WORD(c1,c2,c3,c4)	((U32)c1<<24) | ((U32)c2<<16) | (U32)c3<<8 | c4

#endif /*__DEF_H__*/

