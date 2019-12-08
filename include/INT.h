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
#ifndef __INT_H__
#define __INT_H__

#include "consol.h"

void __HaltUndef(void);
void __HaltSwi(void);
void __HaltPabort(void);
void __HaltDabort(void);
void __HaltFiq(void);
void dummy(int irq);

typedef void (* Exception)(void);
typedef void __irq (* Isr)(void);
void __vInitHaltHandlers(void);
extern Isr ISRVector[32];
void ClearInt(void);

#endif