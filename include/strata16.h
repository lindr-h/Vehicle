#ifndef __STRATA16_H__
#define __STRATA16_H__

#include "def.h"
#include "option.h"
//#include "2410lib.h"

#define BLOCKSIZEOF_STRATA16	(0x20000)

int Strata_Prog(unsigned long data, unsigned long size, unsigned long addr, unsigned long blocks);
int Strata_CheckID(int targetAddr);
int Strata_CheckProtectReg(int targetAddr);
//void Strata_EraseChip(unsigned long uAddr, unsigned long uBlocks)
void Strata_EraseChip(unsigned long uAddr, unsigned long uBlocks);
void Strata_ChipProg(unsigned long ulRomAddr,unsigned long ulRamAddr,unsigned long ulRomSize);

#endif /*__STRATA32_H__*/

