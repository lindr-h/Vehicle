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
/*     RTC.c                                        S3c2410 Realtime 1.0*/
/*                                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains macro and main routines 		 */
/*                                                                       */
/* AUTHOR                                                                */
/*    	Zhi-gang yang   shenzhen watertek                                */
/*                                                                       */
/*                                                                       */
/* DATA STRUCTURES                                                       */
/*                                                                       */
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
/*      2004-12-27      Created initial version 1.0                      */
/*************************************************************************/
#include <string.h>
#include <stdarg.h>
#include "2410addr.h"
#include "def.h"


#include "RTC.h"


void resetRTC(void)
{
	  rRTCCON  = 0x01; 
}

void readRTC(TIME* ptime)
{
	ptime->year 	=2000 + ( rBCDYEAR&0xf0 )*10/16 +( rBCDYEAR&0xf );
	ptime->month 	= ( rBCDMON &0xf0 )*10/16 +( rBCDMON&0xf );
	ptime->day 	= (rBCDDAY&0xf0 )*10/16 +( rBCDDAY&0xf );
	ptime->sec 	= (rBCDSEC &0xf0 )*10/16 +( rBCDSEC&0xf );
	ptime->min 	= (rBCDMIN &0xf0 )*10/16 +( rBCDMIN&0xf );
	ptime->hour 	= (rBCDHOUR &0xf0 )*10/16 +( rBCDHOUR&0xf );
	ptime->weekday 	= (rBCDDATE &0xf0 )*10/16 +( rBCDDATE&0xf );
}

void writeRTC(TIME* ptime)
{
 	rRTCCON = 0x0;
 	rRTCCON = 0x1;
 	rBCDYEAR = (ptime->year)/10*16 + (ptime->year%10);
	rBCDSEC  = (ptime->sec)/10*16 + (ptime->sec%10);
  	rBCDMIN  = (ptime->min)/10*16 + (ptime->min%10);
  	rBCDHOUR = (ptime->hour)/10*16 + (ptime->hour%10);
  	rBCDDAY  = (ptime->day)/10*16 + (ptime->day%10);
  	rBCDDATE = ptime->weekday;
  	rBCDMON  = (ptime->month)/10*16 + (ptime->month%10);
//  	rRTCCON  = 0x01; 
}	

			