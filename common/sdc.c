/*************************************************************************
*
*               Copyright Mentor Graphics Corporation 2003
*                         All Rights Reserved.
*
* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS
* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS
* SUBJECT TO LICENSE TERMS.
*
*************************************************************************/
/*************************************************************************
*
* FILE NAME                                     VERSION
*
*  sdc.c                                Nucleus PLUS\S3C2410X\ADS
*
* DESCRIPTION
*
*  This file contains the Serial Driver specific functions.
*
* DATA STRUCTURES
*
*  SD_PORT *       :   An array of pointers to serial port structures.
*
* FUNCTIONS
*
*  SDC_Init_Port
*  SDC_Date_Ready
*  SDC_Put_String
*  SDC_LISR
*  SDC_Get_Char
*  SDC_Put_Char
*  SDC_Set_Baud_Rate
*
* DEPENDENCIES
*
*  nucleus.h
*  sd_defs.h
*  sd_extr.h
*   target.h
* protocol.h
*  externs.h
*      ppp.h
*
*************************************************************************/
#include "sd_defs.h"
#include "sd_extr.h"
#include "def.h"
#include "2410addr.h"
#include "lcdlib.h"
#include "string.h"

/* Define a small array to hold pointers to the two UART data
   structures. This is used by the LISR to find the correct
   data structure for the interrupt being handled. */
SD_PORT         *SDC_Port_List[SD_MAX_UARTS];

/* Define prototypes for functions local to this module. */
static  VOID    SDC_Set_Baud_Rate(UNSIGNED, SD_PORT *);

/************** Begin Board Specific Section ****************/

/************** End Board Specific Section **************/

/****************************************************************************
* FUNCTION
*
*    SDC_Init_Port
*
* DESCRIPTION
*
*    This function intializes the COM port that will be used for PPP
*    communications.
*
* CALLED BY
*
*    Application
*
* CALLS
*
*    NU_Local_Control_Interrupts
*    SDC_Set_Baud_Rate
*
* INPUTS
*
*    SD_PORT *     :   device initialization structure.
*
* OUTPUTS
*
*    STATUS        :   Returns NU_SUCCESS if successful initialization,
*                      else a negative value is returned.
*
****************************************************************************/
STATUS  SDC_Init_Port(SD_PORT *uart)
{


    STATUS      status = NU_SUCCESS;
    volatile UINT32      temp32;
    UINT32      mode;
    UINT32      submsk;
    static INT  num_ports = 0;

    if (uart->communication_mode == SERIAL_MODE)
    {
        /* Check for max allowed UARTS. */
        if (num_ports >= SD_MAX_UARTS)

           /* We have already initialized the max allowed UARTS. */
           status = NU_UART_LIST_FULL;
    }

    if (status == NU_SUCCESS)
    {
        /* Check the supplied parity */
        if ((uart->parity != SD_PARITY_NONE) &&
             (uart->parity != SD_PARITY_EVEN) &&
             (uart->parity != SD_PARITY_ODD))

        /* The supplied parity is not valid */
        status = NU_INVALID_PARITY;

        /* Check the supplied number of data bits */
        else if ((uart->data_bits != SD_DATA_BITS_5) &&
                 (uart->data_bits != SD_DATA_BITS_6) &&
                 (uart->data_bits != SD_DATA_BITS_7) &&
                 (uart->data_bits != SD_DATA_BITS_8))

            /* The supplied data bits value is not valid */
            status = NU_INVALID_DATA_BITS;

        /* Check the supplied number of stop bits */
        else if ((uart->stop_bits != SD_STOP_BITS_1) &&
                 (uart->stop_bits != SD_STOP_BITS_2))

            /* The supplied stop bits value is not valid */
            status = NU_INVALID_STOP_BITS;

        /* Verify the baud rate is within acceptable range */
        else if ((uart->baud_rate < 1200) || (uart->baud_rate > 115200))

            /* The baud rate is out of range */
            status = NU_INVALID_BAUD;

    /************** Begin Board Specific Section ****************/

        /* Validate the com port. */
        else if ((uart->com_port == SD_UART0) ||
                 (uart->com_port == SD_UART1) ||
                 (uart->com_port == SD_UART2))
        {
            /* Handle UART0 */
            if (uart->com_port == SD_UART0)
            {
                /* Set the vector inside this structure */
                uart->vector = SD_UART0_VECTOR;

                /* Set the base address for this UART. */
                uart->base_address = SD_UART0_BASE;
            }
            /* Handle UART1. */
            else if (uart->com_port == SD_UART1)
            {
                /* Set the vector inside this structure */
                uart->vector = SD_UART1_VECTOR;

                /* Set the base address for this UART. */
                uart->base_address = SD_UART1_BASE;
            }
            /* Handle UART2. */
            else
            {
                /* Set the vector inside this structure */
                uart->vector = SD_UART2_VECTOR;

                /* Set the base address for this UART. */
                uart->base_address = SD_UART2_BASE;
            }

        }
        else

        /************** End Board Specific Section **************/

            /* Not a supported port. */
            status = NU_INVALID_COM_PORT;
    }

    if (uart->communication_mode == SERIAL_MODE)
    {
         /* Make sure all the above was completed. Then store off this
           UART stucture and initialize the chip. */
        if (status == NU_SUCCESS)
        {
            SDC_Port_List[num_ports++] = uart;

        }
    }

    if (status == NU_SUCCESS)
    {
        /* Allocate memory for the data buffers. PPP only requires a TX
           buffer so the allocation will be a little different for PPP mode. */
        if (status == NU_SUCCESS)
        {
            /* Setup the RX SD buffer */
            uart->rx_buffer_read = uart->rx_buffer_write = 0;

            uart->rx_buffer_status = NU_BUFFER_EMPTY;

          }
    }


    if (status == NU_SUCCESS)
    {
        /* Disable interrupts */
//        int_level = NU_Local_Control_Interrupts(NU_DISABLE_INTERRUPTS);

        /************** Begin Board Specific Section *************/

        /* Clear Control registers */
        SD_OUTDWORD (uart->base_address + SD_ULCON_OFFSET, 0);
        SD_OUTDWORD (uart->base_address + SD_UCON_OFFSET, 0);
        SD_OUTDWORD (uart->base_address + SD_UFCON_OFFSET, 0x00000000);
        SD_OUTDWORD (uart->base_address + SD_UMCON_OFFSET, 0x00000000);

        /* Setup baud rate */
        SDC_Set_Baud_Rate(uart->baud_rate, uart);

        /* Setup Mode, Parity, Stop Bits and Data Size in ULCON Reg. */
        SD_OUTBYTE (uart->base_address + SD_ULCON_OFFSET,
            (uart->parity | uart->data_bits | uart->stop_bits));

        /* Enable Rx and Tx in UCON register. */
        SD_OUTWORD (uart->base_address + SD_UCON_OFFSET,
            (SD_UCON_RX_ENABLE | SD_UCON_RX_INT_LEVEL | SD_UCON_TX_INT_LEVEL|SD_UCON_TX_ENABLE));

        /* Get characters from receiver until Receive Ready bit not set */
        do
        {
            temp32 = (volatile)SD_INDWORD(uart->base_address + SD_URXH_OFFSET);

        /* Keep looping until RX ready bit not set */
        } while ((volatile)SD_INDWORD(uart->base_address + SD_UTRSTAT_OFFSET) & SD_USTAT_RX_RDY);

        /* Get current interrupt controller mask register value */
        temp32 = SD_INDWORD (SD_INT_BASE + SD_INT_MSK_OFFSET);

        /* Get current interrupt controller mode register value */
        mode = SD_INDWORD (SD_INT_BASE + SD_INT_MOD_OFFSET);

        /* Get current interrupt sub mask register value */
        submsk = SD_INDWORD (SD_INT_BASE + SD_INT_SUBMSK_OFFSET);

        /* Unmask / set mode in interrupt controller */
        if (uart->com_port == SD_UART0)
        {
            /* Unmask UART0 interrupts (mask and submask) */
            temp32 &= ~SD_INT_UART0;
            submsk &=( ~(SD_INT_RXD0)|SD_INT_TXD0);

            /* Ensure mode bits are cleared first */
            mode &= ~SD_INT_UART0;

            /* Set mode bits based on define in SD_DEFS.H */
            mode |= (SD_INT_UART0 * SD_UART0_INT_MODE);
        }
        else if (uart->com_port == SD_UART1)   /* Otherwise handle UART1. */
        {
            /* Unmask UART1 interrupts */
            temp32 &= ~SD_INT_UART1;
            submsk &= (~(SD_INT_RXD1 )|SD_INT_TXD1);

            /* Ensure mode bits are cleared first */
            mode &= ~SD_INT_UART1;

            /* Set mode bits based on define in SD_DEFS.H */
            mode |= (SD_INT_UART1 * SD_UART1_INT_MODE);
        }
        else /* Otherwise handle UART2. */
        {
            /* Unmask Console UART interrupts */
            temp32 &= ~SD_INT_UART2;
            submsk &= (~(SD_INT_RXD2 )|SD_INT_TXD2);

            /* Ensure mode bits are cleared first */
            mode &= ~SD_INT_UART2;

            /* Set mode bits based on define in SD_DEFS.H */
            mode |= (SD_INT_UART2 * SD_UART2_INT_MODE);
        }

        /* Write updated mask register value back to interrupt controller */
        SD_OUTDWORD (SD_INT_BASE + SD_INT_MSK_OFFSET, temp32);

        /* Write updated mode register value back to interrupt controller */
        SD_OUTDWORD (SD_INT_BASE + SD_INT_MOD_OFFSET, mode);

        /* Write updated interrupt sub mask register value */
        SD_OUTDWORD (SD_INT_BASE + SD_INT_SUBMSK_OFFSET, submsk);

        /************** End Board Specific Section *************/

        /* Initialize the error counters. */
        SDC_Reset(uart);

    }



    /* Return status of initialization */
    return (status);
}


/****************************************************************************
* FUNCTION
*
*    SDC_Put_Char
*
* DESCRIPTION
*
*    This writes a character out to the serial port.
*
* CALLED BY
*
*    UART_Put_String
*    Application
*
* CALLS
*
*    Serial port macros
*
* INPUTS
*
*    UNSIGNED_CHAR :   Character to to be written to the serial port.
*    SD_PORT *     :   Serial port to send the char to.
*
* OUTPUTS
*
*    none
*
****************************************************************************/
VOID  SDC_Put_Char(UINT8 ch, SD_PORT *uart)
{
 
     /* Transmit the character */
    volatile int  status;
      SD_OUTDWORD (uart->base_address + SD_UTXH_OFFSET, ch);
     
      status = SD_INDWORD(uart->base_address + SD_UTRSTAT_OFFSET);
      while(!(status&0x4))
      {
      status = SD_INDWORD(uart->base_address + SD_UTRSTAT_OFFSET);
      }


}

/****************************************************************************
* FUNCTION
*
*    SDC_LISR
*
* DESCRIPTION
*
*    This is the entry function for the ISR that services the UART.
*
* CALLED BY
*
*    none
*
* CALLS
*
*    Serial port macros
*
* INPUTS
*
*    INT         :   Interrupt vector
*
* OUTPUTS
*
*    none
*
****************************************************************************/
void __irq SDC_LISR0(void)
{
	SD_PORT         *uart;
	char            receive,ch,Str[2];
	UINT32          status;
	//INT             vector_found = NU_FALSE;
	int temp;
    
    memset(Str,0,sizeof(Str));	
   

	uart = SDC_Port_List[0];
	/* Ensure vector was found */


	/**************** Begin Board Specific Section **************/

	/* Get the uart status */
	status = SD_INDWORD(uart->base_address + SD_UTRSTAT_OFFSET);

	/* Check if receiver is ready - characters received */
	if (status & SD_USTAT_RX_RDY)
	{
		/* Get UART error status */
		status = SD_INDWORD (uart->base_address + SD_UERSTAT_OFFSET);

		/* Process every character in the receive FIFO */
		do
		{
			/* Clear any error bits */
			SD_OUTDWORD(uart->base_address + SD_UERSTAT_OFFSET,status);

			/* Get the received character from the receive holding register */
			receive = SD_INWORD(uart->base_address + SD_URXH_OFFSET);

			/* Check for parity / framing error */
			if (status & (SD_UERSTAT_FRAME | SD_UERSTAT_OVERRUN))
			{
				/* Update error counts as necessary */
				uart->frame_errors+=((status & SD_UERSTAT_FRAME)==SD_UERSTAT_FRAME);
				uart->parity_errors+=((status & SD_UERSTAT_PARITY)==SD_UERSTAT_PARITY);
			}
			else
			{
				/* Update overrun error count as necessary */
				uart->overrun_errors+=((status & SD_UERSTAT_OVERRUN)==SD_UERSTAT_OVERRUN);

				/* Execute based on mode */
				switch(uart->communication_mode)
				{
					case SERIAL_MODE:

						/* Check if the receive buffer is full */
						if (uart->rx_buffer_status != NU_BUFFER_FULL)
						{
							if(receive == 0x0D)
							{
								PrintTextEdit("$",RGB(255,255,0),5,g_yMax-90,g_xMax-5,30,1);
								SDC_Put_String("\n\rThe received data: ",uart);
								do{
									// Store the character to be returned 
									ch = uart->rx_buffer[uart->rx_buffer_read++];

									SDC_Put_Char(ch,uart);

									// If read pointer is at end, wrap it around 
									if (uart->rx_buffer_read == uart->sd_buffer_size)
										uart->rx_buffer_read = 0;

								} while (uart->rx_buffer_write != uart->rx_buffer_read);
									
								SDC_Put_String("\n\n\rPlease Input the data: ", uart);
							}
							else
							{
								/* Put the character into the buffer */
								uart->rx_buffer[uart->rx_buffer_write++] = receive;

								/*ÏÔÊ¾×Ö·û*/
								Str[0] = receive;
								//Str[1] = 0;
								SDC_Put_Char(receive,uart);
#ifdef	STN
								PrintTextEdit(Str,RGB(255,255,0),5,385,640,30,1);
#else								
								//PrintTextEdit(Str,RGB(255,255,0),5,150,320,30,1);
								PrintTextEdit(Str,RGB(255,255,0),5,g_yMax-90,g_xMax-5,30,1);
#endif								

								/* Check for wrap of buffer. */
								if(uart->rx_buffer_write == uart->sd_buffer_size)
									uart->rx_buffer_write = 0;

								/* Set status field based on latest character */
								if (uart->rx_buffer_write == uart->rx_buffer_read)
									uart->rx_buffer_status = NU_BUFFER_FULL;
								else
									uart->rx_buffer_status = NU_BUFFER_DATA;
							}
						}
						else
							/* buffer is full, increment busy errors */
							uart->busy_errors++;
						break;

				}

			}

			/* Get UART status */
			status = SD_INDWORD (uart->base_address + SD_UTRSTAT_OFFSET);

			/* Keep looping while receive data ready */
		} while (status& SD_USTAT_RX_RDY);//

	/**************** End Board Specific Section **************/
	}

	temp = rSUBSRCPND;
	rSUBSRCPND=temp;
	temp = rSRCPND;
	rSRCPND = temp;
	temp = rINTPND;
	rINTPND = temp;
 }

void __irq SDC_LISR1(void)
{
    SD_PORT         *uart;
    CHAR            receive;
    UINT32          status;
    INT             vector_found,temp;
    	
   
    vector_found = TRUE;
    uart = SDC_Port_List[1];
    /* Ensure vector was found */
    if (vector_found == TRUE)
    {
        /**************** Begin Board Specific Section **************/

        /* Get the uart status */
        status = SD_INDWORD(uart->base_address + SD_UTRSTAT_OFFSET);

        /* Check if receiver is ready - characters received */
        if (status & SD_USTAT_RX_RDY)
        {
            /* Get UART error status */
            status = SD_INDWORD (uart->base_address + SD_UERSTAT_OFFSET);

            /* Process every character in the receive FIFO */
            do
            {
                /* Clear any error bits */
                SD_OUTDWORD(uart->base_address + SD_UERSTAT_OFFSET,status);

                /* Get the received character from the receive holding register */
                receive = (UINT8)SD_INDWORD(uart->base_address + SD_URXH_OFFSET);

                /* Check for parity / framing error */
                if (status & (SD_UERSTAT_FRAME | SD_UERSTAT_OVERRUN))
                {
                    /* Update error counts as necessary */
                    uart->frame_errors+=((status & SD_UERSTAT_FRAME)==SD_UERSTAT_FRAME);
                    uart->parity_errors+=((status & SD_UERSTAT_PARITY)==SD_UERSTAT_PARITY);
                }
                else
                {
                    /* Update overrun error count as necessary */
                    uart->overrun_errors+=((status & SD_UERSTAT_OVERRUN)==SD_UERSTAT_OVERRUN);

                    /* Execute based on mode */
                    switch(uart->communication_mode)
                    {
                        case SERIAL_MODE:

                            /* Check if the receive buffer is full */
                            if (uart->rx_buffer_status != NU_BUFFER_FULL)
                            {
                               /* Put the character into the buffer */
                               uart->rx_buffer[uart->rx_buffer_write++] = receive;
                               /*ÏÔÊ¾×Ö·û*/
                               SDC_Put_Char(receive,uart);

                               /* Check for wrap of buffer. */
                               if(uart->rx_buffer_write == uart->sd_buffer_size)
                                   uart->rx_buffer_write = 0;

                               /* Set status field based on latest character */
                               if (uart->rx_buffer_write == uart->rx_buffer_read)
                                   uart->rx_buffer_status = NU_BUFFER_FULL;
                               else
                                   uart->rx_buffer_status = NU_BUFFER_DATA;
                            }
                            else
                                /* buffer is full, increment busy errors */
                                uart->busy_errors++;
                        break;

                    }

                }

                /* Get UART status */
                status = SD_INDWORD (uart->base_address + SD_UTRSTAT_OFFSET);

            /* Keep looping while receive data ready */
            } while (status & SD_USTAT_RX_RDY);

            /**************** End Board Specific Section **************/
        }


         }
        temp = rSUBSRCPND;
        rSUBSRCPND=temp;
	temp = rSRCPND;
	rSRCPND = temp;
	temp = rINTPND;
	rINTPND = temp;

  
   
 }

void __irq SDC_LISR2(void)
{
    SD_PORT         *uart;
    CHAR            receive;
    UINT32          status;
    INT             vector_found,temp;
	
    	
   
    vector_found = TRUE;
    uart = SDC_Port_List[2];
    /* Ensure vector was found */
    if (vector_found == TRUE)
    {
        /**************** Begin Board Specific Section **************/

        /* Get the uart status */
        status = SD_INDWORD(uart->base_address + SD_UTRSTAT_OFFSET);

        /* Check if receiver is ready - characters received */
        if (status & SD_USTAT_RX_RDY)
        {
            /* Get UART error status */
            status = SD_INDWORD (uart->base_address + SD_UERSTAT_OFFSET);

            /* Process every character in the receive FIFO */
            do
            {
                /* Clear any error bits */
                SD_OUTDWORD(uart->base_address + SD_UERSTAT_OFFSET,status);

                /* Get the received character from the receive holding register */
                receive = (UINT8)SD_INDWORD(uart->base_address + SD_URXH_OFFSET);

                /* Check for parity / framing error */
                if (status & (SD_UERSTAT_FRAME | SD_UERSTAT_OVERRUN))
                {
                    /* Update error counts as necessary */
                    uart->frame_errors+=((status & SD_UERSTAT_FRAME)==SD_UERSTAT_FRAME);
                    uart->parity_errors+=((status & SD_UERSTAT_PARITY)==SD_UERSTAT_PARITY);
                }
                else
                {
                    /* Update overrun error count as necessary */
                    uart->overrun_errors+=((status & SD_UERSTAT_OVERRUN)==SD_UERSTAT_OVERRUN);

                    /* Execute based on mode */
                    switch(uart->communication_mode)
                    {
                        case SERIAL_MODE:

                            /* Check if the receive buffer is full */
                            if (uart->rx_buffer_status != NU_BUFFER_FULL)
                            {
                               /* Put the character into the buffer */
                               uart->rx_buffer[uart->rx_buffer_write++] = receive;
                               /*ÏÔÊ¾×Ö·û*/
                               SDC_Put_Char(receive,uart);

                               /* Check for wrap of buffer. */
                               if(uart->rx_buffer_write == uart->sd_buffer_size)
                                   uart->rx_buffer_write = 0;

                               /* Set status field based on latest character */
                               if (uart->rx_buffer_write == uart->rx_buffer_read)
                                   uart->rx_buffer_status = NU_BUFFER_FULL;
                               else
                                   uart->rx_buffer_status = NU_BUFFER_DATA;
                            }
                            else
                                /* buffer is full, increment busy errors */
                                uart->busy_errors++;
                        break;

                    }

                }

                /* Get UART status */
                status = SD_INDWORD (uart->base_address + SD_UTRSTAT_OFFSET);

            /* Keep looping while receive data ready */
            } while (status & SD_USTAT_RX_RDY);

            /**************** End Board Specific Section **************/
        }


         }
        temp = rSUBSRCPND;
        rSUBSRCPND=temp;
	temp = rSRCPND;
	rSRCPND = temp;
	temp = rINTPND;
	rINTPND = temp;

  
   
 }



/****************************************************************************
* FUNCTION
*
*    SDC_Set_Baud_Rate
*
* DESCRIPTION
*
*    This function sets the UART buad rate.
*
* CALLED BY
*
*    SDC_Init_Port
*
* CALLS
*
*    Serial port macros
*    NU_Local_Control_Interrupts
*
* INPUTS
*
*    UNSIGNED      :  The new baud rate.
*    SD_PORT *     :  Serial port to set the baud rate.
*
* OUTPUTS
*
*    none
*
****************************************************************************/
static VOID SDC_Set_Baud_Rate(UNSIGNED baud_rate, SD_PORT *uart)
{
    UINT32  baud_div;           /* baud rate divisor */


    /**************** Begin Board Specific Section **************/

    /* Update baud rate in uart structure */
    uart->baud_rate = 115200;
    //uart->baud_rate = 57600;  /*Èç¹ûbaud_div = 0x36*/

    /* Calculate the baud rate divisor */
    baud_div =0x1B;
    
    //baud_div = 0x36;

    /* Put baud rate divisor in baud divisor register */
    SD_OUTDWORD (uart->base_address + SD_UBRDIV_OFFSET, baud_div);

    /**************** End Board Specific Section ****************/

}


/****************************************************************************
* FUNCTION
*
*    SDC_Get_Char
*
* DESCRIPTION
*
*    This function reads the last received character from the UART.
*
* CALLED BY
*
*    Application
*
* CALLS
*
*    none
*
* INPUTS
*
*    SD_PORT *      :   Serial port to get the char from.
*
* OUTPUTS
*
*    CHAR  :  Character read
*
****************************************************************************/
CHAR  SDC_Get_Char(SD_PORT *uart)
{
    CHAR    ch = 0;
     if (uart->communication_mode == SERIAL_MODE)
    {
        if (uart->rx_buffer_status != NU_BUFFER_EMPTY)
        {
            /* Store the character to be returned */
            ch = uart->rx_buffer[uart->rx_buffer_read++];
         
            /* If read pointer is at end, wrap it around */
            if (uart->rx_buffer_read == uart->sd_buffer_size)
                uart->rx_buffer_read = 0;

            /* Set the status to reflect removal of the character */
            if (uart->rx_buffer_write == uart->rx_buffer_read)
                uart->rx_buffer_status = NU_BUFFER_EMPTY;
            else
                uart->rx_buffer_status = NU_BUFFER_DATA;
        }

    } /* endif mode */

    /* Return the character to the caller */
    return (ch);
}


/****************************************************************************
* FUNCTION
*
*    SDC_Carrier
*
* DESCRIPTION
*
*    This function checks for a carrier.
*
* CALLED BY
*
*    MDM_Hangup
*
* CALLS
*
*    none
*
* INPUTS
*
*    none
*
* OUTPUTS
*
*    STATUS    :  The status of the detection.
*
****************************************************************************/
STATUS SDC_Carrier(SD_PORT *uart)
{
    return (TRUE);
}

/****************************************************************************
 Note: All functions below this point are generic and should not require
       any changes to support other UARTS.
 ****************************************************************************/

/****************************************************************************
* FUNCTION
*
*    SDC_Put_String
*
* DESCRIPTION
*
*    This writes a null-terminated string out to the serial port.
*
* CALLED BY
*
*    Application
*
* CALLS
*
*    SDC_Put_Char
*
* INPUTS
*
*    CHAR *        :   String to be written to the serial port.
*    SD_PORT *     :   Serial port to send the string to.
*
* OUTPUTS
*
*    none
*
****************************************************************************/
VOID SDC_Put_String(CHAR *str, SD_PORT *uart)
{

    /* Grab the semaphore so that strings between threads
       do not get mixed. */
    {

        /* Send out the string. */
        for (; *str != 0; str++)
            SDC_Put_Char(*str, uart);

        /* Allow other threads to use this service. */

    }

}


/****************************************************************************
* FUNCTION
*
*    SDC_Data_Ready
*
* DESCRIPTION
*
*    This function checks to see if there are any characters in the
*    receive buffer.  A status value is returned indicating whether
*    characters are present in the receive buffer.
*
* CALLED BY
*
*    Application
*
* CALLS
*
*    none
*
* INPUTS
*
*    SD_PORT *      :   Serial port to check for data.
*
* OUTPUTS
*
*    STATUS                                The status indicates the
*                                          presence of characters.
*
****************************************************************************/
STATUS SDC_Data_Ready(SD_PORT *port)
{
    /* Check the status. */
    if((port->rx_buffer_status == NU_BUFFER_FULL) ||
       (port->rx_buffer_status == NU_BUFFER_DATA))

        return (TRUE);

    else

        return (FALSE);
}

/****************************************************************************
* FUNCTION
*
*    SDC_Change_Communication_Mode
*
* DESCRIPTION
*
*    This function switches the serial port between terminal mode and
*    network mode.  The mode affects how incoming characters are directed.
*
* CALLED BY
*
*    MDM_Change_Communication_Mode
*
* CALLS
*
*    none
*
* INPUTS
*
*    INT      :  The mode of operation desired.
*
* OUTPUTS
*
*    none
*
****************************************************************************/
VOID SDC_Change_Communication_Mode(INT mode, SD_PORT *uart)
{
    uart->communication_mode = mode;

} /* SDC_Change_Communication_Mode */

/****************************************************************************
* FUNCTION
*
*    SDC_Reset
*
* DESCRIPTION
*
*    This function intializes the data variables associated with a UART
*
* CALLED BY
*
*    PPP_Dial
*    PPP_Wait_For_Client
*
* CALLS
*
*    none
*
* INPUTS
*
*    SD_PORT      * :   Serial port to reset
*
* OUTPUTS
*
*    STATUS      :   Returns URT_SUCCESS if successful initialization,
*                    else a negative value is returned.
*
****************************************************************************/
VOID SDC_Reset (SD_PORT *uart)
{
    /* Ini the error counters */
    uart->frame_errors   = 0;
    uart->overrun_errors = 0;
    uart->parity_errors  = 0;
    uart->busy_errors    = 0;
    uart->general_errors = 0;
}

/***************************************************************************
* FUNCTION
*
*    URT_Init_Port
*
* DESCRIPTION
*
*    This function intializes the data variables associated with a UART
*
* CALLED BY
*
*    PPP_Dial
*    PPP_Wait_For_Client
*
* CALLS
*
*    none
*
* INPUTS
*
*    SD_PORT      * :   Serial port to reset
*
* OUTPUTS
*
*    STATUS      :   Returns URT_SUCCESS if successful initialization,
*                    else a negative value is returned.
*
****************************************************************************/
#ifdef NU_ENABLE_PPP
STATUS  URT_Init_Port(DV_DEVICE_ENTRY *device)
{
    SD_PORT   *uart;
    STATUS    ret_status;

    /* Get a pointer to the UART layer of this device. */
    uart = &((PPP_LAYER *) device->ppp_layer)->uart;

    /* Init the serial port, copy init parameters from the device
       structure. */
    uart->com_port              = device->dev_com_port;
    uart->baud_rate             = device->dev_baud_rate;
    uart->data_bits             = device->dev_data_bits;
    uart->stop_bits             = device->dev_stop_bits;
    uart->parity                = device->dev_parity;
    uart->data_mode             = device->dev_data_mode;
    uart->driver_options        = device->dev_driver_options;
    uart->communication_mode    = MDM_TERMINAL_COMMUNICATION;
    uart->sd_buffer_size        = (2 * (PPP_MTU + PPP_FCS_SIZE +
                                    PPP_MAX_PROTOCOL_SIZE + PPP_MAX_ADDR_CONTROL_SIZE));

    /* Init the port */
    ret_status = NU_SD_Init_Port (uart);

    if (ret_status == NU_SUCCESS)
    {
        /* Copy the vector assigned by the driver into
           the device's structure. */
        device->dev_vect = uart->tx_vector;
    }

    return (ret_status);

}
#endif /* NU_ENABLE_PPP */


