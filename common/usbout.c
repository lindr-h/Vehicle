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
/*      USBout.c                                   S3c2410 USB device 1.0*/
/*                                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains macro and routines for Endpoint out		 */
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
#include "option.h"
#include "2410addr.h"
#include "def.h"

#include "2410usb.h"
#include "usbmain.h"
#include "usb.h"
#include "usblib.h"
#include "usbsetup.h"
#include "usbout.h"

#include "u241mon.h"


static void PrintEpoPkt(U8 *pt,int cnt);
//static void RdPktEp3_CheckSum(U8 *buf,int num);



// ===================================================================
// All following commands will operate in case 
// - out_csr3 is valid.
// ===================================================================

//clear EP3 out packet ready tag 

#define CLR_EP3_OUT_PKT_READY() rOUT_CSR1_REG= ( out_csr3 &(~ EPO_WR_BITS)\
					&(~EPO_OUT_PKT_READY) ) 

//MCU issues a STALL handshake to the USB					
#define SET_EP3_SEND_STALL()	rOUT_CSR1_REG= ( out_csr3 & (~EPO_WR_BITS)\
					| EPO_SEND_STALL) )
//clear Ep3 sent stall(host -> MCU)					
#define CLR_EP3_SENT_STALL()	rOUT_CSR1_REG= ( out_csr3 & (~EPO_WR_BITS)\
					&(~EPO_SENT_STALL) )
//flush ep3 FIFO					
#define FLUSH_EP3_FIFO() 	rOUT_CSR1_REG= ( out_csr3 & (~EPO_WR_BITS)\
					|EPO_FIFO_FLUSH) )

// ***************************
// *** VERY IMPORTANT NOTE ***
// ***************************
// Prepare for the packit size constraint!!!

// EP3 = OUT end point. 

U8 ep3Buf[EP3_PKT_SIZE];
//static U8 tempBuf[64+1];
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Ep3Handler		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      Ep3 interrupt handler				                 */
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
void Ep3Handler(void)
{
    U8 out_csr3;
    int fifoCnt;
    rINDEX_REG=3;

    out_csr3=rOUT_CSR1_REG;		//get ep3 status
    

    if(out_csr3 & EPO_OUT_PKT_READY)	//if USB load a packet into Ep3 FIFO
    {   
	fifoCnt=rOUT_FIFO_CNT1_REG; 

	RdPktEp3(ep3Buf,fifoCnt);	//read data from ep3 fifo
	PrintEpoPkt(ep3Buf,fifoCnt);
   	CLR_EP3_OUT_PKT_READY();	//clear EP3 out packet ready tag 
	return;
    }

    
    //I think that EPO_SENT_STALL will not be set to 1.
    if(out_csr3 & EPO_SENT_STALL)	//if stall handshake is loaded into Ep3 FIFO
    {   
   	CLR_EP3_SENT_STALL();		//clear Ep3 stall handshake tag
   	return;
    }	
}

//print ep out packet for debug,if you want to get data from host to device,
//you should modify this function

void PrintEpoPkt(U8 *pt,int cnt)
{
//    int i;
//    DbgPrintf("[BOUT:%d:",cnt);
//    for(i=0;i<cnt;i++)
//    	DbgPrintf("%x,",pt[i]);
//    DbgPrintf("]");
}

//DMA interrupt handler ,no use in this case
void IsrDma2(void)
{

}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      ClearEp3OutPktReady		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      clear Ep3 out packet read tag				                 */
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
void ClearEp3OutPktReady(void)
{
    U8 out_csr3;
    rINDEX_REG=3;
    out_csr3=rOUT_CSR1_REG;
    CLR_EP3_OUT_PKT_READY();
}
