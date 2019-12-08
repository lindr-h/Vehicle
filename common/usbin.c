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
/*      usbin.c                                    S3c2410 USB device 1.0*/
/*                                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains data structure definitions , constants 	 */
/*	and manipulation facilitiesused  in 2410 ep1(in) 		 */     
/* AUTHOR                                                                */
/*    	Zhi-gang yang   shenzhen watertek                                */
/*                                                                       */
/* DATA STRUCTURES                                                       */
/*                                                                       */
/*      ep1Buf[EP1_PKT_SIZE]                  IN ENDPOINT Buffer         */
/*      transferIndex           	      transfer Counter   	 */
/*                                                                       */
/* FUNCTIONS                                                             */
/*                                                                       */
/*      PrepareEp1Fifo                        prepare ep1 FIFO and setup */
/*					      ep1 already, if token IN 	 */
/*					      arrive, data send to host	 */
/*      Ep1Handler                            ep1 interrupt handler	 */
/*      PrintEpiPkt                           print ep1 packet,only for  */
/*						debug 			 */
/*                                                                       */
/* DEPENDENCIES                                                          */
/*                                                                       */
/*      option.h	                       				 */
/*      string.h	                       				 */
/*      2410addr.h                          				 */
/*      2410usb.h                                                        */
/*      2410lib.h                         				 */
/*      def.h                                                            */
/* HISTORY                                                               */
/*                                                                       */
/*         DATE                    REMARKS                               */
/*                                                                       */
/*      2005-1-8       Created initial version 1.0                       */
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
#include "usbin.h"


static void PrintEpiPkt(U8 *pt,int cnt);


// ===================================================================
// All following commands will operate in case 
// - in_csr1 is valid.
// ===================================================================
//set EP1 IN packet ready tag
#define SET_EP1_IN_PKT_READY()  rIN_CSR1_REG= ( in_csr1 &(~ EPI_WR_BITS)\
 					| EPI_IN_PKT_READY )
 //MCU issues a STALL handshake to the USB	 
#define SET_EP1_SEND_STALL()	rIN_CSR1_REG= ( in_csr1 & (~EPI_WR_BITS)\
					| EPI_SEND_STALL) )
//clear Ep3 sent stall(host -> MCU)
#define CLR_EP1_SENT_STALL()	rIN_CSR1_REG= ( in_csr1 & (~EPI_WR_BITS)\
					&(~EPI_SENT_STALL) )
////flush ep3 FIFO
#define FLUSH_EP1_FIFO() 	rIN_CSR1_REG= ( in_csr1 & (~EPI_WR_BITS)\
					| EPI_FIFO_FLUSH) )


// ***************************
// *** VERY IMPORTANT NOTE ***
// ***************************
// Prepare the code for the packit size constraint!!!

// EP1 = IN end point. 

U8 ep1Buf[EP1_PKT_SIZE];
int transferIndex=0;

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      PrepareEp1Fifo		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      prepare ep1 fifo for data send			                 */
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
void PrepareEp1Fifo(void) 
{
    int i;
    U8 in_csr1;
    rINDEX_REG=1;
    in_csr1=rIN_CSR1_REG;
    
    for(i=0;i<EP1_PKT_SIZE;i++)ep1Buf[i]=(U8)(transferIndex+i);
    WrPktEp1(ep1Buf,EP1_PKT_SIZE);
    SET_EP1_IN_PKT_READY(); 
}
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Ep1Handler		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      Ep1 interrupt handler				                 */
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

void Ep1Handler(void)
{
    U8 in_csr1;
    rINDEX_REG=1;
    in_csr1=rIN_CSR1_REG;
    

    //I think that EPI_SENT_STALL will not be set to 1.
    if(in_csr1 & EPI_SENT_STALL)
    {   
   	CLR_EP1_SENT_STALL();
   	return;
    }	

    //IN_PKT_READY is cleared
    
    //The data transfered was ep1Buf[] which was already configured 

    PrintEpiPkt(ep1Buf,EP1_PKT_SIZE); 
    
    transferIndex++;

    PrepareEp1Fifo(); 
    	//IN_PKT_READY is set   
    	//This packit will be used for next IN packit.	

    return;
}
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      PrintEpiPkt		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      print epi packet for debug. if read data (mcu--->host)		 */
/*      it should be modified.                                           */
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

    
void PrintEpiPkt(U8 *pt,int cnt)
{
/*    int i;
    DbgPrintf("[B_IN:%d:",cnt);
    for(i=0;i<cnt;i++)
    	DbgPrintf("%x,",pt[i]);
    DbgPrintf("]");
    */
}

