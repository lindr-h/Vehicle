#ifndef __ROMDEF_H__
#define __ROMDEF_H__

#define ManuIdAddr      (0x0000)
#define DevIdAddr       (0x0001)

#define RomSize		ROM2M
	#define ROM4M	(0x40000)
	#define ROM2M	(0x200000)
	#define ROM1M	(0x100000)

#define SeqAddr1	(0x5555)
#define SeqCom1		(0xaa)

#define SeqAddr2	(0x2aaa)
#define SeqCom2		(0x55)

//The following is CFI command
#define SoftIdEntry	(0x90)
#define WordProg	(0xa0)
#define Erase		(0x80)
	#define ChipErase	(0x10)

#define RamBaseAddr	(0x0c200000)
#endif  // __ROMDEF_H //
