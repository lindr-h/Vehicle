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
/*   pwm.h                                 S3c2410 stepping motor device 1.0 */
/*                                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains  constants used  in the stepping motor         */
/* AUTHOR                                                                */
/*    	Li-li zhang   shenzhen watertek                                */
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
/*      2005-8-15      Created initial version 1.0                      */
/*************************************************************************/

#ifndef PWM_H
#define PWM_H

#define TIMER2  (1)
#define TIMER3  (2)
#define MOTORPCLK     (50700000)
#define PRESCALE  (255)
#define DIVIDER   (2)
#define LEVEL  (2)	


 void __irq TIMER2_LISR(void);
 void MotorOn(void);
 void MotorOff(void);
 void DirectoryCtrl(int flag);
 void AngleCtrl(int flag);
 void SetTimer(void);
 void SetClkFreq(int freq);
 void DirectoryCtrl(int flag);

#endif/* PWM_H */
