#ifndef NAND_FLASH
#define NAND_FLASH

#define BAD_CHECK	(0)
#define ECC_CHECK	(0)
 

#define WRITEVERIFY  (0)  

#define NF_CMD(cmd)	{rNFCMD=cmd;}
#define NF_ADDR(addr)	{rNFADDR=addr;}	
#define NF_nFCE_L()	{rNFCONF&=~(1<<11);}
#define NF_nFCE_H()	{rNFCONF|=(1<<11);}
#define NF_RSTECC()	{rNFCONF|=(1<<12);}
#define NF_RDDATA() 	(rNFDATA)
#define NF_WRDATA(data) {rNFDATA=data;}

#define NF_WAITRB()    {while(!(rNFSTAT&(1<<0)));} 
	   
#define ID_K9S1208V0M	0xec76

#if 1
// HCLK=100Mhz
#define TACLS		0  //1clk(0ns) 
#define TWRPH0		3  //3clk(25ns)
#define TWRPH1		0  //1clk(10ns)  //TACLS+TWRPH0+TWRPH1>=50ns
#else
// HCLK=50Mhz
#define TACLS		0  //1clk(0ns)
#define TWRPH0		1  //2clk(25ns)
#define TWRPH1		0  //1clk(10ns)
#endif



void NF_Init(void);

void K9S1208_Program(U32 blockIndex, U32 srcAddress, U32 fileSize);
void K9S1208_read(U32 block, U32 srcAddress, U32 fileSize);

void K9S1208_Program(U32 blockIndex, U32 srcAddress, U32 fileSize);


U16 NF_CheckId(void);
int NF_EraseBlock(U32 blockNum);
int NF_ReadPage(U32 block,U32 page,U8 *buffer);
int NF_WritePage(U32 block,U32 page,U8 *buffer);
	//buffer size is 512 bytes
int NF_IsBadBlock(U32 block);
int NF_MarkBadBlock(U32 block);
void NF_Reset(void);
#endif