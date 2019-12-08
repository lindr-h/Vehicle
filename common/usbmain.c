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
/*      usbmain.c                                   S3c2410 USB device 1.0*/
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
/*      Ep3Handler                   EP3 interrupt handler          	 */
/*      PrintEpoPkt                  print ep out packet for debug       */
/*      RdPktEp3_CheckSum            No	use 				 */
/*      IsrDma2              	     DMA interrupt handler,No use in this*/
/*					 case  				 */
/*      ClearEp3OutPktReady          clear endpoint 3 packet out tag     */
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
#include "option.h"
#include "2410addr.h"
//#include "2410lib.h"
#include "def.h"
#include "lcdlib.h"

#include "2410usb.h"
#include "usbmain.h"
#include "usblib.h"
#include "usbsetup.h"
#include "usbout.h"
#include "usbin.h"

unsigned short rgb = 0;

void ChangeUPllValue(int mdiv,int pdiv,int sdiv);

/**************************
    Some PrepareEp1Fifo() should be deleted
 **************************/   
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      UsbdMain		                                        		 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      usb main function				                				 */
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
void UsbdMain(void)
{
    ChangeUPllValue(40,1,2);  //UCLK=48Mhz  
    //init descriptor table
    InitDescriptorTable();
    //ResetUsbd();
    
    ConfigUsbd(); 

    //prepare data for ep1
    PrepareEp1Fifo(); 
  
   
}
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      IsrUsbd		               		                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      usb interrupt handler				                 */
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

void __irq IsrUsbd(void)
{
    U8 usbdIntpnd,epIntpnd;
    U8 saveIndexReg=rINDEX_REG;
    usbdIntpnd=rUSB_INT_REG;
    epIntpnd=rEP_INT_REG;

    if(usbdIntpnd&SUSPEND_INT)
    {
    	rUSB_INT_REG=SUSPEND_INT;	//clear suspend interrupt
    }
    if(usbdIntpnd&RESUME_INT)		//clear resume interrupt
    {
    	rUSB_INT_REG=RESUME_INT;
    }
    if(usbdIntpnd&RESET_INT)		//reconfig usb and clear reset interrupt
    {
    	
    	PrintTextEdit("$检测到USB主设备",rgb,5,g_yMax-90,g_xMax,30,1);
    	rgb += 0x10;
    	//ResetUsbd();
    	ReconfigUsbd();
		
    	rUSB_INT_REG=RESET_INT;  //RESET_INT should be cleared after ResetUsbd().   	

        PrepareEp1Fifo(); 
    }

    if(epIntpnd&EP0_INT)		//clear End point out interrupt  and setup usb
    {
	rEP_INT_REG=EP0_INT;  
    	Ep0Handler();
    }
    if(epIntpnd&EP1_INT)		//clear End point 1 interrupt handler and implemented Ep1handler
    {
    	rEP_INT_REG=EP1_INT;  
    	Ep1Handler();
    }

    if(epIntpnd&EP2_INT)
    {
    	rEP_INT_REG=EP2_INT;  
  		 //not implemented yet	
    	//Ep2Handler();
    }

    if(epIntpnd&EP3_INT)		//clear End point 1 interrupt handler and implemented Ep1handler
    {
    	rEP_INT_REG=EP3_INT;
    	Ep3Handler();
    }

    if(epIntpnd&EP4_INT)
    {
    	rEP_INT_REG=EP4_INT;
		 //not implemented yet	
    	//Ep4Handler();
    }

    ClearPending(BIT_USBD);	 	//clear USB interrupt
    
    rINDEX_REG=saveIndexReg;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      ChangeUPllValue		       		                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      chang UPLL					                 */
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



void ChangeUPllValue(int mdiv,int pdiv,int sdiv)
{
    rUPLLCON = (mdiv<<12) | (pdiv<<4) | sdiv;
}

