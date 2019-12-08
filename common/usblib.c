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
/*      USBlib.c                                   S3c2410 USB device 1.0*/
/*                                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains  constants used  in the USB protocol          */
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
/*     string.h                                                          */
/*     option.h                                                          */
/*     2410addr.h                                                        */
/*     def.h                                                             */
/*     2410usb.h                                                         */
/*     usbmain.h                                                         */
/*     usblib.h                                                          */
/*     usbsetup.h                                                        */
/*     usbmain.h                                                         */
/*                                                                       */
/* HISTORY                                                               */
/*                                                                       */
/*         DATE                    REMARKS                               */
/*                                                                       */
/*      2004-12-22      Created initial version 1.0                      */
/*************************************************************************/

#include <string.h>
#include "option.h"
#include "2410addr.h"
#include "def.h"

#include "2410usb.h"
#include "usbmain.h"
#include "usblib.h"
#include "usbsetup.h"
#include "usbmain.h"
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      ConfigUsbd		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      config usb device function				                 */
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

void ConfigUsbd(void)
{
    //reconfig usb device
    ReconfigUsbd();
    //enable usb device interrupt
    rINTMSK&=~(BIT_USBD);  
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      ReconfigUsbd		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      config usb device function				                 */
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

void ReconfigUsbd(void)
{
// *** End point information ***
//   EP0: control
//   EP1: bulk in end point
//   EP2: not used
//   EP3: bulk out end point
//   EP4: not used
    
    rPWR_REG=PWR_REG_DEFAULT_VALUE;	//disable suspend mode

    rINDEX_REG=0;	
    rMAXP_REG=FIFO_SIZE_8;   	//EP0 max packit size = 8 
    rEP0_CSR=EP0_SERVICED_OUT_PKT_RDY|EP0_SERVICED_SETUP_END;	
 				//EP0:clear OUT_PKT_RDY & SETUP_END
    rINDEX_REG=1;
    #if (EP1_PKT_SIZE==32)
        rMAXP_REG=FIFO_SIZE_32;	//EP1:max packit size = 32
    #else
	rMAXP_REG=FIFO_SIZE_64;	//EP1:max packit size = 64
    #endif	
    rIN_CSR1_REG=EPI_FIFO_FLUSH|EPI_CDT;	
    rIN_CSR2_REG=EPI_MODE_IN|EPI_IN_DMA_INT_MASK|EPI_BULK; //IN mode, IN_DMA_INT=masked    
    rOUT_CSR1_REG=EPO_CDT;   	
    rOUT_CSR2_REG=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	

    rINDEX_REG=2;
    rMAXP_REG=FIFO_SIZE_64;	//EP2:max packit size = 64
    rIN_CSR1_REG=EPI_FIFO_FLUSH|EPI_CDT|EPI_BULK;
    rIN_CSR2_REG=EPI_MODE_IN|EPI_IN_DMA_INT_MASK; //IN mode, IN_DMA_INT=masked    
    rOUT_CSR1_REG=EPO_CDT;   	
    rOUT_CSR2_REG=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	

    rINDEX_REG=3;
    #if (EP3_PKT_SIZE==32)
        rMAXP_REG=FIFO_SIZE_32;	//EP3:max packit size = 32
    #else
	rMAXP_REG=FIFO_SIZE_64;	//EP3:max packit size = 64
    #endif	
    rIN_CSR1_REG=EPI_FIFO_FLUSH|EPI_CDT|EPI_BULK;
    rIN_CSR2_REG=EPI_MODE_OUT|EPI_IN_DMA_INT_MASK; //OUT mode, IN_DMA_INT=masked    
    rOUT_CSR1_REG=EPO_CDT;   	
    	//clear OUT_PKT_RDY, data_toggle_bit.
	//The data toggle bit should be cleared when initialization.
    rOUT_CSR2_REG=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	

    rINDEX_REG=4;
    rMAXP_REG=FIFO_SIZE_64;	//EP4:max packit size = 64
    rIN_CSR1_REG=EPI_FIFO_FLUSH|EPI_CDT|EPI_BULK;
    rIN_CSR2_REG=EPI_MODE_OUT|EPI_IN_DMA_INT_MASK; //OUT mode, IN_DMA_INT=masked    
    rOUT_CSR1_REG=EPO_CDT;   	
    	//clear OUT_PKT_RDY, data_toggle_bit.
	//The data toggle bit should be cleared when initialization.
    rOUT_CSR2_REG=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	
    
    rEP_INT_REG=EP0_INT|EP1_INT|EP2_INT|EP3_INT|EP4_INT;
    rUSB_INT_REG=RESET_INT|SUSPEND_INT|RESUME_INT; 
    	//Clear all usbd pending bits
    	
    //EP0,1,3 & reset interrupt are enabled
    rEP_INT_EN_REG=EP0_INT|EP1_INT|EP3_INT;
    rUSB_INT_EN_REG=RESET_INT;
    ep0State=EP0_STATE_INIT;
    
}
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      RdPktEp0		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      read packet Ep0 from Ep0 fifo			                 */
/*                                                                       */
/* AUTHOR                                                                */
/*                                                                       */
/*      Zhi-gang Yang          shenzhen watertek                         */
/*                                                                       */
/* INPUTS                                                                */
/*      buf		       data buffer                               */
/*      num			data size (byte)                                                                 */
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


void RdPktEp0(U8 *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        buf[i]=(U8)rEP0_FIFO;	
    }
}
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      WrPktEp0		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      write packet  into Ep0 fifo			                 */
/*                                                                       */
/* AUTHOR                                                                */
/*                                                                       */
/*      Zhi-gang Yang          shenzhen watertek                         */
/*                                                                       */
/* INPUTS                                                                */
/*      buf		       data buffer                               */
/*      num			data size (byte)                         */
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
void WrPktEp0(U8 *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        rEP0_FIFO=buf[i];	
    }
}
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      WrByteEp0		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      write byte  into Ep0 fifo			                 */
/*                                                                       */
/* AUTHOR                                                                */
/*                                                                       */
/*      Zhi-gang Yang          shenzhen watertek                         */
/*                                                                       */
/* INPUTS                                                                */
/*      value			write byte into ep0 fifo                 */
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

void WrByteEp0(U8 value)
{
  rEP0_FIFO= value;
}
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      WrPktEp1		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      write packet  into Ep1 fifo			                 */
/*                                                                       */
/* AUTHOR                                                                */
/*                                                                       */
/*      Zhi-gang Yang          shenzhen watertek                         */
/*                                                                       */
/* INPUTS                                                                */
/*      buf		       data buffer                               */
/*      num			data size (byte)                         */
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



void WrPktEp1(U8 *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        rEP1_FIFO=buf[i];	
    }
}
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      WrPktEp2		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      write packet  into Ep2 fifo			                 */
/*                                                                       */
/* AUTHOR                                                                */
/*                                                                       */
/*      Zhi-gang Yang          shenzhen watertek                         */
/*                                                                       */
/* INPUTS                                                                */
/*      buf		       data buffer                               */
/*      num			data size (byte)                         */
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


void WrPktEp2(U8 *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        rEP2_FIFO=buf[i];	
    }
}
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      RdPktEp3		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      read packet  into Ep3 fifo			                 */
/*                                                                       */
/* AUTHOR                                                                */
/*                                                                       */
/*      Zhi-gang Yang          shenzhen watertek                         */
/*                                                                       */
/* INPUTS                                                                */
/*      buf		       data buffer                               */
/*      num			data size (byte)                         */
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

void RdPktEp3(U8 *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        buf[i]=(U8)rEP3_FIFO;	
    }
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      RdPktEp4		                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      read packet  from Ep4 fifo			                 */
/*                                                                       */
/* AUTHOR                                                                */
/*                                                                       */
/*      Zhi-gang Yang          shenzhen watertek                         */
/*                                                                       */
/* INPUTS                                                                */
/*      buf		       data buffer                               */
/*      num			data size (byte)                         */
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
void RdPktEp4(U8 *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        buf[i]=(U8)rEP4_FIFO;	
    }
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      ConfigEp3DmaMode	                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      config Ep3 transmit by DMA mode			                 */
/*                                                                       */
/* AUTHOR                                                                */
/*                                                                       */
/*      Zhi-gang Yang          shenzhen watertek                         */
/*                                                                       */
/* INPUTS                                                                */
/*      bufAdr		       data buffer address                       */
/*      count			data size (byte)                         */
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
void ConfigEp3DmaMode(U32 bufAddr,U32 count)
{
    int i;

    rINDEX_REG=3;
    count=count&0xfffff; //transfer size should be <1MB
    
    rDISRCC2=(1<<1)|(1<<0);
    rDISRC2=ADDR_EP3_FIFO; //src=APB,fixed,src=EP3_FIFO 
    rDIDSTC2=(0<<1)|(0<<0);
    rDIDST2=bufAddr;       //dst=AHB,increase,dst=bufAddr
#if USBDMA_DEMAND
    rDCON2=(count)|(0<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(4<<24)|(1<<23)|(0<<22)|(0<<20); 
        //demand,requestor=APB,CURR_TC int enable,unit transfer,
        //single service,src=USBD,H/W request,autoreload,byte,CURR_TC
#else
    rDCON2=(count)|((unsigned)1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(4<<24)|(1<<23)|(0<<22)|(0<<20); 
        //handshake,requestor=APB,CURR_TC int enable,unit transfer,
        //single service,src=USBD,H/W request,autoreload,byte,CURR_TC
#endif        
    rDMASKTRIG2= (1<<1); 
        //DMA 2 on

    //rEP3_DMA_FIFO=0x40; //not needed for OUT operation. 	

    rEP3_DMA_TTC_L=0xff;
    rEP3_DMA_TTC_M=0xff;
    rEP3_DMA_TTC_H=0x0f;

    rOUT_CSR2_REG=rOUT_CSR2_REG|EPO_AUTO_CLR|EPO_OUT_DMA_INT_MASK; 
    	//AUTO_CLR(OUT_PKT_READY is cleared automatically), interrupt_masking.
#if USBDMA_DEMAND
    rEP3_DMA_UNIT=EP3_PKT_SIZE; //DMA transfer unit=64 bytes
    rEP3_DMA_CON=UDMA_DEMAND_MODE|UDMA_OUT_DMA_RUN|UDMA_DMA_MODE_EN; 
        // deamnd enable,out_dma_run=run,in_dma_run=stop,DMA mode enable
#else        
    rEP3_DMA_UNIT=0x01; //DMA transfer unit=1byte
    rEP3_DMA_CON=UDMA_OUT_DMA_RUN|UDMA_DMA_MODE_EN;
        // deamnd disable,out_dma_run=run,in_dma_run=stop,DMA mode enable
#endif  
    //wait until DMA_CON is effective.
//    rEP3_DMA_CON;
    for(i=0;i<10;i++);    	
}
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      ConfigEp3IntMode	                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      config Ep3 transmit by interrupt mode			         */
/*                                                                       */
/* AUTHOR                                                                */
/*                                                                       */
/*      Zhi-gang Yang          shenzhen watertek                         */
/*                                                                       */
/* INPUTS                                                                */
/*      bufAdr		       data buffer address                       */
/*      count			data size (byte)                         */
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

void ConfigEp3IntMode(void)
{
    rINDEX_REG=3;
    
    rDMASKTRIG2= (0<<1);  // EP3=DMA ch 2
        //DMA channel off
    rOUT_CSR2_REG=rOUT_CSR2_REG&~(EPO_AUTO_CLR/*|EPO_OUT_DMA_INT_MASK*/); 
    	//AUTOCLEAR off,interrupt_enabled (???)
    rEP3_DMA_UNIT=1;	
    rEP3_DMA_CON=0; 
  	// deamnd disable,out_dma_run=stop,in_dma_run=stop,DMA mode disable
    //wait until DMA_CON is effective.
    
}
