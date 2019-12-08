/**************************************************************************
*                                                                         *
*   PROJECT     :                                     			  *
*                                                                         *
*   MODULE      :                                                         *
*                                                                         *
*   AUTHOR      :                                                         *
*                 URL  :                                                  *
*                 EMAIL:                                                  *
*                                                                         *
*   PROCESSOR   : S3c2410x (32 bit ARM920T RISC core from Samsung)        *
*                                                                         *
*   IDE         : SDT 2.51 & ADS 1.2                                      *
*                                                                         *
*   DESCRIPTION :                                                         *
*                                                                         *
*                                                                         *
**************************************************************************/
#include "consol.h"
#include "S3c2410x.h"
#include "INT.h"
#include "consol.h"
#include <stdarg.h>
#include <stdio.h>

//#include "led.h"

Isr ISRVector[32];

/* ********************************************************************* */
/* Local functions */

/*
*********************************************************************************************
*                                       __Haltx
*
* Description: These exception handlers are used to report a particular exception on the
*              CONSOL & halt.
*
* Arguments  : none.
*
* Return     : none.
*
* Note(s)    : 
*********************************************************************************************
*/

void __HaltUndef(void)
{
	CONSOL_Printf("Undefined instruction exception!!!\n");
	while(1);
}

void __HaltSwi(void)
{
	CONSOL_Printf("SWI exception!!!\n");
	while(1);
}

void __HaltPabort(void)
{
	CONSOL_Printf("Pabort exception!!!\n");
	while(1);
}

void __HaltDabort(void)
{
	CONSOL_Printf("Dabort exception !!!\n");
	while(1);
}

void __HaltFiq(void)
{
	CONSOL_Printf("FIQ exception!!!\n");
	while(1);
/*
	while(1)
	{
		Led_D1(TRUE);
        Led_delay(500);
        Led_D1(FALSE);
        Led_delay(500);
	}
*/
}

void dummy(int irq)
{
	CONSOL_Printf("IRQ exception!!!\n");
	while(1);
}




void __vInitHaltHandlers(void)
{
	extern void IsrIRQ(void);
	extern Exception Handle[];
	int i;

	Handle[0] = __HaltUndef;
	Handle[1] =__HaltSwi;
	Handle[2] =__HaltPabort;
	Handle[3] =__HaltDabort;
	Handle[5] =IsrIRQ;
	Handle[6] =__HaltFiq;

	for(i=0;i<32;i++)
		ISRVector[i]=(Isr)dummy;

	rINTMOD     = 0x0;				//All=IRQ mode
	rINTMSK     = BIT_ALLMSK;		//All interrupt is masked.
	rINTSUBMSK  = BIT_SUB_ALLMSK;	//All sub-interrupt is masked. <- April 01, 2002 SOP
}

void ClearInt(void)
{
	int temp;
	
	temp = rSUBSRCPND;
    rSUBSRCPND = temp;
	temp = rSRCPND;
	rSRCPND = temp;
	temp = rINTPND;
	rINTPND = temp;
}