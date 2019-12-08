#ifndef SST_H
#define SST_H
#include "typDef.h"

UINT16 *sstWrite(UINT16 *flashAddr, UINT8 *buffer, ULONG length);
STATUS sstChipErase(void);
STATUS sstSectorErase(UINT16 *pSector);
//void downloadflash(unsigned short* startaddr, long len);
//void flash_Entry(void);
#define SST_START_ADDR          0x08000000
#define SST_CHIP_HWORD_SIZE     0x100000                 /*1M Hwords*/
#define SST_SECTOR_HWORD_SIZE   0x800                 /*2k HWords*/

#define SST_ADDR_UNLOCK1        0x5555
#define SST_ADDR_UNLOCK2        0x2aaa
#define SST_DATA_UNLOCK1        0xaaaa
#define SST_DATA_UNLOCK2        0x5555
#define SST_SETUP_WRITE         0xa0a0
#define SST_SETUP_ERASE         0x8080
#define SST_CHIP_ERASE          0x1010
#define SST_SECTOR_ERASE		0x3030

#endif