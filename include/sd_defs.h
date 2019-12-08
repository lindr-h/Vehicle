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
/*      sd_defs.h                             Nucleus PLUS\S3C2410X\ADS  */
/*                                                                       */
/* COMPONENT                                                             */
/*                                                                       */
/*      SD - Serial Driver                                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains constant definitions and function macros      */
/*      for the Serial Driver module.                                    */
/*                                                                       */
/* DATA STRUCTURES                                                       */
/*                                                                       */
/*      SD_PORT     :   Structure to keep all needed info. about a port. */
/*                                                                       */
/* DEPENDENCIES                                                          */
/*                                                                       */
/*      none                                                             */
/*                                                                       */
/*************************************************************************/
#ifndef SD_DEFS_H
#define SD_DEFS_H


#define UNSIGNED unsigned int
#define CHAR char
#define INT int
#define STATUS int
#define VOID void
#define UINT8 unsigned char
#define UINT32 unsigned int
#define UINT16 unsigned short
#define NU_SUCCESS 1
#define NU_FALSE 0

/**************** User configurable section *************************/

/* Define default Serial Driver settings for this board */
#define DEFAULT_UART_PORT       UART0
#define DEFAULT_UART_BAUD       115200
#define DEFAULT_UART_DATA       DATA_BITS_8
#define DEFAULT_UART_STOP       STOP_BITS_1
#define DEFAULT_UART_PARITY     PARITY_NONE
#define DEFAULT_UART_MODE       MODE_NORMAL
#define DEFAULT_UART_BUFFER     1000

/* The following defines are used to set-up the interrupt
   type for each of the available UARTS (0 = IRQ / 1 = FIQ) */
#define SD_UART0_INT_MODE       0
#define SD_UART1_INT_MODE       0
#define SD_UART2_INT_MODE       0

/**************** End configurable section *************************/

/* Interrupt Registers */
#define SD_INT_BASE             0x4A000000
#define SD_INT_MOD_OFFSET       0x04        /* Internal Interrupt Mode Register */
#define SD_INT_MSK_OFFSET       0x08        /* Internal Interrupt Mask Register */
#define SD_INT_PND_OFFSET       0x10        /* Internal Interrupt Source Pending Register */
#define SD_INT_SUBSRCPND_OFFSET 0x18        /* Internal Interrupt sub source pending register */
#define SD_INT_SUBMSK_OFFSET    0x1C        /* Internal Interrupt sub source pending register */

/* Interrupt Register values */
#define SD_INT_RXD0             0x01
#define SD_INT_TXD0             0x02
#define SD_INT_ERR0             0x04
#define SD_INT_RXD1             0x08
#define SD_INT_TXD1             0x10
#define SD_INT_ERR1             0x20
#define SD_INT_RXD2             0x40
#define SD_INT_TXD2             0x80
#define SD_INT_ERR2             0x100

#define SD_SUBSRCPND_TX         ((SD_INT_TXD0) | (SD_INT_TXD1) | (SD_INT_TXD2))
#define SD_SUBSRCPND_RX         ((SD_INT_RXD0) | (SD_INT_RXD1) | (SD_INT_RXD2))
#define SD_SUBSRCPND_ERR        ((SD_INT_ERR0) | (SD_INT_ERR1) | (SD_INT_ERR2))

/* Interrupt Vector Numbers */
#define SD_UART0_VECTOR         28
#define SD_UART1_VECTOR         23
#define SD_UART2_VECTOR         15

/* UART Interrupt Mask Register Bits */
#define SD_INT_UART0            0x10000000
#define SD_INT_UART1            0x00800000
#define SD_INT_UART2            0x00008000

/* The base addresses for the seperate UART registers. */
#define SD_UART0_BASE           0x50000000
#define SD_UART1_BASE           0x50004000
#define SD_UART2_BASE           0x50008000

/* Macros for specifying which UART to use.
   NOTE: The defines must start at 1 to avoid
   problems with Nuclues C++ */
#define SD_UART0                1
#define SD_UART1                2
#define SD_UART2                3
#define SD_MAX_UARTS            3

/* UART register offsets from the UART base */
#define SD_ULCON_OFFSET         0x00        /* UART Line Control */
#define SD_UCON_OFFSET          0x04        /* UART Channel Control Register */
#define SD_UFCON_OFFSET         0x08        /* UART FIFO Control Register */
#define SD_UMCON_OFFSET         0x0C        /* UART Modem Control Register */
#define SD_UTRSTAT_OFFSET       0x10        /* UART Tx/Rx Status Register */
#define SD_UERSTAT_OFFSET       0x14        /* UART Rx error Status Register */
#define SD_UFSTAT_OFFSET        0x18        /* UART FIFO Status Register */
#define SD_UMSTAT_OFFSET        0x1C        /* UART Modem Status Register */
#define SD_UTXH_OFFSET          0x20        /* UART Trasmit Buffer Register */
#define SD_URXH_OFFSET          0x24        /* UART Receive Buffer Register */
#define SD_UBRDIV_OFFSET        0x28        /* UART Baud Rate Divisor Register */

/* UART Line Control Register Bits */
#define SD_DATA_BITS_5          0x00000000  /* 5 bits per frame */
#define SD_DATA_BITS_6          0x00000001  /* 6 bits per frame */
#define SD_DATA_BITS_7          0x00000002  /* 7 bits per frame */
#define SD_DATA_BITS_8          0x00000003  /* 8 bits per frame */

#define SD_STOP_BITS_1          0x00000000  /* One stop bit per frame */
#define SD_STOP_BITS_2          0x00000004  /* Two stop bits per frame */

#define SD_PARITY_NONE          0x00000000  /* No parity */
#define SD_PARITY_ODD           0x00000020  /* Odd parity */
#define SD_PARITY_EVEN          0x00000028  /* Even parity */
#define SD_PARITY_FORCED_1      0x00000030  /* Parity forced checked as 1 */
#define SD_PARITY_FORCED_0      0x00000038  /* Parity forced checked as 0 */

/* UART Control Register Bits */
#define SD_UCON_RX_DISABLE      0x00000000  /* Rx disabled */
#define SD_UCON_RX_ENABLE       0x00000001  /* Rx interrupt request or polling mode */
#define SD_UCON_RX_MASK         0x00000003  /* Mask for the receive mode bits */
#define SD_UCON_TX_DISABLE      0x00000000  /* Tx disabled */
#define SD_UCON_TX_ENABLE       0x00000004  /* Tx interrupt request or polling mode */
#define SD_UCON_TX_MASK         0x0000000C  /* Mask for the transmit mode bits */
#define SD_UCON_MODE_NORMAL     0x00000000  /* UART runs in normal operation */
#define SD_UCON_MODE_LOOPBACK   0x00000020  /* UART runs in loopback mode */
#define SD_UCON_ERROR_DISABLE   0x00000000  /* Do not generate receive error status interrupt */
#define SD_UCON_ERROR_ENABLE    0x00000040  /* Generate receive error status interrupt */
#define SD_UCON_TIMEOUT_DISABLE 0x00000000  /* Disable Rx time out interrupt when UART FIFO enabled */
#define SD_UCON_TIMEOUT_ENABLE  0x00000080  /* Enable Rx time out interrupt when UART FIFO enabled */
#define SD_UCON_RX_INT_PULSE    0x00000000  /* Receive interrupt request is Pulse */
#define SD_UCON_RX_INT_LEVEL    0x00000100  /* Receive interrupt request is Level */
#define SD_UCON_TX_INT_PULSE    0x00000000  /* Transmit interrupt request type is Pulse */
#define SD_UCON_TX_INT_LEVEL    0x00000200  /* Transmit interrupt request type is Level */
#define SD_UCON_PCLK            0x00000000  /* Use PCLK for baud rate, UBRDIVn=(PCLK/(bpsx16))-1 */
#define SD_UCON_UCLK            0x00000400  /* Use UCLK for baud rate, UBRDIVn=(UCLK/(bpsx16))-1 */

/* UART FIFO Control Register Bits */
#define SD_UFCON_ENABLE         0x00000001  /* FIFO Enable */
#define SD_UFCON_RX_RESET       0x00000002  /* RX FIFO Reset */
#define SD_UFCON_TX_RESET       0x00000004  /* TX FIFO Reset */
#define SD_UFCON_RX_TRG_8       0x00000010  /* RX FIFO Trigger 8 byte */
#define SD_UFCON_RX_TRG_12      0x00000020  /* RX FIFO Trigger 12 byte */
#define SD_UFCON_RX_TRG_16      0x00000030  /* RX FIFO Trigger 16 byte */
#define SD_UFCON_TX_TRG_4       0x00000040  /* TX FIFO Trigger 4 byte */
#define SD_UFCON_TX_TRG_8       0x00000080  /* TX FIFO Trigger 8 byte */
#define SD_UFCON_TX_TRG_12      0x000000C0  /* TX FIFO Trigger 12 byte */

/* UART Status Register Bits */
#define SD_USTAT_RESET          0x00000006  /* Status reset */
#define SD_USTAT_RX_RDY         0x00000001  /* Receive Data Valid */
#define SD_USTAT_TX_RDY         0x00000002  /* Transmit Register Empty */
#define SD_USTAT_TX_EMPTY       0x00000004  /* Tramsmit buffer ans shifter register empty */

/* UART Error Status Register */
#define SD_UERSTAT_OVERRUN      0x00000001  /* Overrun error */
#define SD_UERSTAT_PARITY       0x00000002  /* Parity error */
#define SD_UERSTAT_FRAME        0x00000004  /* Frame error */
#define SD_UERSTAT_BREAK        0x00000008  /* Break receive */

/* Define data structures for management of a serial port. */
typedef struct SD_INIT_STRUCT
{
    UNSIGNED        data_mode;
    UNSIGNED        base_address;

    /* The following elements should generic accross other
       platforms. */
    UNSIGNED        com_port;
    UNSIGNED        data_bits;
    UNSIGNED        stop_bits;
    UNSIGNED        parity;
    UNSIGNED        baud_rate;
/************** Begin Port Specific Section ****************/
    UNSIGNED        vector;
/*************** End Port Specific Section *****************/
    UNSIGNED        driver_options;
    UNSIGNED        sd_buffer_size;

    UNSIGNED        parity_errors;
    UNSIGNED        frame_errors;
    UNSIGNED        overrun_errors;
    UNSIGNED        busy_errors;
    UNSIGNED        general_errors;

    CHAR            rx_buffer[4096];
    INT             rx_buffer_read;
    INT             rx_buffer_write;
    volatile INT    rx_buffer_status;

    /* All of the following elements are required by PPP, do not modify. */
    UNSIGNED        communication_mode;

} SD_PORT;

/* Defines to be used by application */
#define MODE_NORMAL         SD_UCON_MODE_NORMAL
#define MODE_LOOP_BACE      SD_UCON_MODE_LOOPBACK

#define STOP_BITS_1         SD_STOP_BITS_1
#define STOP_BITS_2         SD_STOP_BITS_2

#define UART0               SD_UART0
#define UART1               SD_UART1
#define UART2               SD_UART2

/* Defines to determine communication mode */
#define SERIAL_MODE                 0
#define MDM_NETWORK_COMMUNICATION   1
#define MDM_TERMINAL_COMMUNICATION  2
#define POLLING_MODE                3

/****************************************************************/
/* Note: everything below should be genric.                     */
/****************************************************************/

#define NU_SERIAL_PORT          SD_PORT
#define PARITY_NONE             SD_PARITY_NONE
#define PARITY_EVEN             SD_PARITY_EVEN
#define PARITY_ODD              SD_PARITY_ODD

#define DATA_BITS_5             SD_DATA_BITS_5
#define DATA_BITS_6             SD_DATA_BITS_6
#define DATA_BITS_7             SD_DATA_BITS_7
#define DATA_BITS_8             SD_DATA_BITS_8

#define NU_SD_Put_Char          SDC_Put_Char
#define NU_SD_Get_Char          SDC_Get_Char
#define NU_SD_Put_String        SDC_Put_String
#define NU_SD_Init_Port         SDC_Init_Port
#define NU_SD_Data_Ready        SDC_Data_Ready

#define NU_UART_SUCCESS         0
#define NU_INVALID_PARITY       -1
#define NU_INVALID_DATA_BITS    -2
#define NU_INVALID_STOP_BITS    -3
#define NU_INVALID_BAUD         -4
#define NU_INVALID_COM_PORT     -5
#define NU_INVALID_DATA_MODE    -6
#define NU_UART_LIST_FULL       -7

#define SDC_BUFFER_FULL          1
#define SDC_BUFFER_DATA          2
#define SDC_BUFFER_EMPTY         3
#define NU_BUFFER_FULL	SDC_BUFFER_FULL
#define NU_BUFFER_DATA SDC_BUFFER_DATA
#define NU_BUFFER_EMPTY SDC_BUFFER_EMPTY	

/* Deifine IO macros. */

/* 8 bit access */
#define SD_OUTBYTE(reg, data)   ( (*( (UINT8 *) (reg) ) ) = (UINT8)(data) )

#define SD_INBYTE(reg)          (  *( (UINT8 *) (reg) ) )

/* 16 bit access */
#define SD_OUTWORD(reg, data)   ( (*( (UINT16 *) (reg) ) ) = (UINT16)(data) )

#define SD_INWORD(reg)          (  *( (UINT16 *) (reg) ) )

/* 32 bit access */
#define SD_OUTDWORD(reg, data)  ( (*( (UINT32 *) (reg) ) ) = (UINT32)(data) )

#define SD_INDWORD(reg)         (  *( (UINT32 *) (reg) ) )

/*Macro used for converting URT to SD_PORT. This is for PPP serial driver
  backwards compatability. */
#define URT_LAYER                       SD_PORT

#define URT_TX_BUFFER_SIZE              uart->sd_buffer_size
#define URT_Get_Char                    SDC_Get_Char
#define URT_Put_Char                    SDC_Put_Char
#define URT_Reset                       SDC_Reset
#define URT_Change_Communication_Mode   SDC_Change_Communication_Mode
#define URT_Carrier                     SDC_Carrier


#endif /* ifndef SD_DEFS_H */


