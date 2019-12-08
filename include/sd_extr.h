/*************************************************************************/
/*                                                                       */
/*               Copyright Mentor Graphics Corporation 2003              */
/*                         All Rights Reserved.                          */
/*                                                                       */
/* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS  */
/* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS   */
/* SUBJECT TO LICENSE TERMS.                                             */
/*                                                                       */
/*************************************************************************/

/*************************************************************************/
/*                                                                       */
/* FILE NAME                                        VERSION              */
/*                                                                       */
/*      sd_extr.h                             Nucleus PLUS\S3C2410X\ADS  */
/*                                                                       */
/* COMPONENT                                                             */
/*                                                                       */
/*      SD - Serial Driver                                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains function prototypes for the Serial Driver     */
/*  module.                                                              */
/*                                                                       */
/* DATA STRUCTURES                                                       */
/*                                                                       */
/*      none                                                             */
/*                                                                       */
/* DEPENDENCIES                                                          */
/*                                                                       */
/*      sd_defs.h                                                        */
/*                                                                       */
/*************************************************************************/
#ifndef SD_EXTR
#define SD_EXTR

#include "sd_defs.h"

/* SDC function prototypes */
STATUS  SDC_Init_Port(SD_PORT *);
VOID    SDC_Put_Char(UINT8, SD_PORT *);
VOID    SDC_Put_String(CHAR *, SD_PORT *);
//CHAR  SDC_Get_Char(SD_PORT *uart,char *p,long len);
CHAR  SDC_Get_Char(SD_PORT *uart);
STATUS  SDC_Data_Ready(SD_PORT *);
VOID    SDC_Change_Communication_Mode(INT, SD_PORT *);
STATUS  SDC_Carrier(SD_PORT *);
VOID    SDC_Reset (SD_PORT *);
void __irq SDC_LISR0(void);
void __irq SDC_LISR1(void);
void __irq SDC_LISR2(void);
#endif





