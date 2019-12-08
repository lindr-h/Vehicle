//====================================================================
// File Name : strata32.c
// Function  : S3C2410 Intel Strata NOR Flash
// Program   : Lee, Sang Jo (LSJ)
// Date      : June 14, 2002
// Version   : 0.0
// History
//   0.0 : Programming start (June 14, 2002) -> LSJ
//         Arrangement source code(8/01/2002)-> SOP
//====================================================================

#include <stdlib.h>
#include <string.h>
//#include "misc.h"
#include "strata16.h"
#include "2410lib.h"
#include "2410addr.h"

#define STRATA16_INTEL		0x89
#define STRATA16_32MBIT		0x16
#define STRATA16_64MBIT		0x17
#define STRATA16_128MBIT	0x18

//extern void Delay(INT32 time);
//extern void Blink(INT32 times);
//extern void LedDisp(INT32 i, INT32 on);

//High-Density Symmetrical 128-Kbyte Blocks
const int Block_Number[3] = {
	32		//32 Mbit (32 Blocks)
	, 64	//64 Mbit (64 Blocks)
	, 128	//128 Mbit (128 Blocks)
};

//static int  Strata_CheckID(int targetAddr);
static int  Strata_CheckDevice(int targetAddr);

// by eric rong
void _WR(int addr,INT16U data)
{
	*((volatile INT16U *)(addr))=data ;
}

INT16U _RD(int addr)
{
	return ( *((volatile INT16U *)(addr)) );
}

void _RESET()
{
#ifndef _XCHG_CGSx
	_WR(0x0,0x00ff);
#else
	_WR(0x8000000,0x00ff);
#endif
}

//High-Density Symmetrical 128-Kbyte Blocks
//128 Mbit (128 Blocks)
//64 Mbit (64 Blocks)
//32 Mbit (32 Blocks)
int Strata_ClearLock(int targetAddr)
{
    unsigned long ReadStatus;
    unsigned long bSR5;		// Erase and Clear Lock-bits Status, lower 16bit, 8MB Intel Strate Flash ROM
    unsigned long bSR7;		// Write State Machine Status, lower 16bit, 8MB Intel Strate Flash ROM

//    targetAddr &= ~(Block_Number[0] * 1024 * 1024);
	_WR(targetAddr,0x0060);
	_WR(targetAddr,0x00d0);

	do{
	    //_RESET();
	    _WR(targetAddr, 0x0070);  // Read Status Register
	    ReadStatus=_RD(targetAddr);   // realAddr is any valid address within the device
	    bSR7=ReadStatus & (1<<7);
    }while(!bSR7);

    _WR(targetAddr, 0x0070);		// When the block erase is complete, status register bit SR.5 should be checked.
					// If a block erase error is detected, the status register should be cleared before
					// system software attempts correct actions.
    ReadStatus=_RD(targetAddr);
    bSR5=ReadStatus & (1<<5);           // lower 16-bit 8MB Strata
	if (0!=bSR5)
	{
		//Uart_Printf("Error in Block Erasure!!\n");
		_WR(targetAddr, 0x0050);	// Clear Status Register
	}

	//Uart_Printf("Block_%x Erase O.K. \n",targetAddress);
	return 0;
}

//==========================================================================================
int Strata_CheckID(int targetAddr)
{
    //_RESET();
    char iID = 0;
    targetAddr &= ~(Block_Number[0] * 1024 * 1024);
    _WR(targetAddr, 0x0090);

    iID = _RD(targetAddr);
    _RESET();
    return iID;
	//return _RD(targetAddr);
				// Read Identifier Code, including lower, higher 16-bit, 8MB, Intel Strate Flash ROM
				// targetAddress must be the beginning location of a Block Address
				// test ok by eric rong 2003-4-6
}

//==========================================================================================
int Strata_CheckDevice(int targetAddr)
{
    //_RESET();
    int iDev = 0;
    targetAddr &= ~(Block_Number[0] * 1024 * 1024);
    _WR(targetAddr, 0x0090);
    
    iDev = _RD(targetAddr+0x2);
    _RESET();
    return iDev;
//	return _RD(targetAddr+0x2);
    			// Read Device Code, including lower, higher 16-bit, 8MB, Intel Strate Flash ROM
				// targetAddress must be the beginning location of a Block Address
				// test ok by eric rong 2003-4-6
}

unsigned short usProtectReg[9];
int Strata_CheckProtectReg(int targetAddr)
{
	int i;
	_WR(targetAddr,0x0090);
	for(i=0;i<9;i++)
	{
		usProtectReg[i]=_RD(targetAddr+0x80+(i*2));
	}
    _RESET();
	return 0;
}

//=========================================================================================
void Strata_EraseSector(int targetAddress)
{
    unsigned long ReadStatus;
    unsigned long bSR5;		// Erase and Clear Lock-bits Status, lower 16bit, 8MB Intel Strate Flash ROM
    unsigned long bSR7;		// Write State Machine Status, lower 16bit, 8MB Intel Strate Flash ROM

	Strata_ClearLock(targetAddress);

    _WR(targetAddress, 0x0020);	// Block Erase, First Bus Cycle, targetAddress is the address withint the block
    _WR(targetAddress, 0x00d0);	// Block Erase, Second Bus Cycle, targetAddress is the address withint the block

	do{
	    //_RESET();
	    _WR(targetAddress, 0x0070);  // Read Status Register
	    ReadStatus=_RD(targetAddress);   // realAddr is any valid address within the device
	    bSR7=ReadStatus & (1<<7);
    }while(!bSR7);

    _WR(targetAddress, 0x0070);		// When the block erase is complete, status register bit SR.5 should be checked.
					// If a block erase error is detected, the status register should be cleared before
					// system software attempts correct actions.
    ReadStatus=_RD(targetAddress);
    bSR5=ReadStatus & (1<<5);           // lower 16-bit 8MB Strata
    if (0!=bSR5)
    {
        //Uart_Printf("Error in Block Erasure!!\n");
        _WR(targetAddress, 0x0050);	// Clear Status Register
    }

	//Uart_Printf("Block_%x Erase O.K. \n",targetAddress);
	_RESET();   // write 0xffh(_RESET()) after the last opoeration to reset the device to read array mode.
}

void Strata_EraseChip(unsigned long uAddr, unsigned long uBlocks)
{
	INT32U uErasedBlocks = 0;
	INT32U targetAddress = uAddr;

	for(; uErasedBlocks < uBlocks; uErasedBlocks++)
	{
//		Blink(1);

		Strata_EraseSector(targetAddress);
		targetAddress += BLOCKSIZEOF_STRATA16;
	}
}

//==============================================================================================================
void Strata_ProgFlash(INT32U realAddr,INT16U data)
{
    volatile INT16U *ptargetAddr;
    unsigned long ReadStatus;
    unsigned long bSR4;    // Erase and Clear Lock-bits Status, lower 16bit, 8MB Intel Strate Flash ROM
    unsigned long bSR7;    // Write State Machine Status, lower 16bit, 8MB Intel Strate Flash ROM

    ptargetAddr = (volatile INT16U *)realAddr;
    //_RESET();

    _WR(realAddr, 0x0040);  // realAddr is any valid adress within the device
                                // Word/Byte Program(or 0x00100010 can be used)
    *ptargetAddr=data;          // 32 bit data

	do{
	    //_RESET();
	    _WR(realAddr, 0x0070);  // Read Status Register
	    ReadStatus=_RD(realAddr);   // realAddr is any valid address within the device
	    bSR7=ReadStatus & (1<<7);
    }while(!bSR7);

    _WR(realAddr, 0x0070);
    ReadStatus=_RD(realAddr);             // Real Status Register
    bSR4=ReadStatus & (1<<4);

    if (0 != bSR4)
    {
        //Uart_Printf("Error Program!!\n");
        _WR(realAddr, 0x0050);          // Clear Status Register
    }

	//Uart_Printf("Successful Program!!\n");
    _RESET();
}

void Strata_ChipProg(unsigned long ulRomAddr,unsigned long ulRamAddr,unsigned long ulRomSize)
{
	INT16U usData = 0xff;
	
	while(ulRomSize)
	{
		usData=*(volatile INT16U *)ulRamAddr;
		Strata_ProgFlash(ulRomAddr,usData);

		ulRomSize-=2;
		ulRamAddr+=2;
		ulRomAddr+=2;

	//	if (0 == (ulRamAddr % (BLOCKSIZEOF_STRATA16 / 4)))
//			Blink(2);
	}
}

int Strata_Prog(unsigned long data, unsigned long size, unsigned long addr, unsigned long blocks)
{
	volatile INT16U ID;

	Strata_CheckProtectReg(addr);
	if (STRATA16_INTEL != (ID=Strata_CheckID(addr)))
		return 0;

	ID=Strata_CheckDevice(addr);
	if (ID < STRATA16_32MBIT || STRATA16_128MBIT < ID)
		return 0;

	if (-1 == blocks)
	{
		blocks = Block_Number[ID-STRATA16_32MBIT];
#if !defined(_XCHG_CGSx)
		blocks -= addr / BLOCKSIZEOF_STRATA16;
#else
		blocks -= (addr - 0x8000000) / BLOCKSIZEOF_STRATA16;
#endif // _XCHG_CGSx
	}

#if !defined(_XCHG_CGSx)
	if (Block_Number[ID-STRATA16_32MBIT] < (addr / BLOCKSIZEOF_STRATA16 + blocks))
#else
	if (Block_Number[ID-STRATA16_32MBIT] < ((addr - 0x8000000) / BLOCKSIZEOF_STRATA16 + blocks))
#endif // _XCHG_CGSx
		return 0;

	Strata_EraseChip(addr, blocks);	//I think this can be remove
	Delay(100);

	if (0 == size)
		return ID;

//	Blink(3);
	Strata_ChipProg(addr,data,size);

	return ID;
}



/*
void Delay(INT32 time)
// time=0: adjust the Delay function by WatchDog timer.
// 100us resolution.
{
    INT32 i=0;
    INT32 delayLoopCount= 400;
    for(;time>0;time--)
	for(i=0;i<delayLoopCount;i++); 
}
*/