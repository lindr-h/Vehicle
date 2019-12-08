
#include "typDef.h"
#include "sst39vf160.h"
#include "2410lib.h"

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


/******************************************************
函数名称:       sstOpOverDetect()
函数功能:       采用poll方式检测flash擦写是否完成.
入口参数:       ptr         数据写入地址/擦除扇区首址.
				trueData    要写入的值.
				timeCounter 超时计数.
返 回 值:       OK		操作成功.
				ERROR	操作失败.
备    注:       在预定时间内如果d7,d6仍不是truedata,则返回
				ERROR.
*******************************************************/    
STATUS  sstOpOverDetect(UINT16 *ptr,  UINT16 trueData, ULONG timeCounter)
{
	ULONG timeTmp = timeCounter;
	volatile UINT16 *pFlash = ptr;
	UINT16 buf1, buf2,curTrueData;
	
	curTrueData = trueData & 0x8080;                  //先检测d7位.
	while((*pFlash & 0x8080) != curTrueData)
	{
		if(timeTmp-- == 0) break;
	}
	
	timeTmp = timeCounter;
	buf1 = *pFlash & 0x4040;						  //(为保险)再检测d6位.
	while(1)
	{
		    buf2  = *pFlash & 0x4040;
		    if(buf1 == buf2) 
				break;
		    else
			    buf1 = buf2;
		    if(timeTmp-- == 0) 
		    {
				return ERROR;
		    }
	}

	return OK;
}

/********************************************************
函数名称:       sstWrite()
函数功能:       读取缓冲区数据根据给定的长度写入指定地址.
入口参数:       flashAddr    数据目标地址(flash).
				buffer       数据源地址.
				length 		 要写入的字节数.
				
返 回 值:      	NULL		 写失败.
				flashPtr     flash的下一个地址.
备    注:       由于sst39vf160只能按半字(16bit)操作,所以
                如果要多次调用这个函数来写入一个文件,则应
                每次读取偶数个字节,以保证连续性.
*********************************************************/    

UINT16 *sstWrite(UINT16 *flashAddr, UINT8 *buffer, ULONG length)
{
	ULONG i, cLength;
	volatile UINT16 *flashPtr;
	volatile UINT16 *gBuffer;
	
    flashPtr = flashAddr;
	cLength = (length + 1)/2;				//计算半字长度.
	gBuffer = (UINT16 *)buffer;      
	
	while (cLength > 0) 
	{
		*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_DATA_UNLOCK1;          //解锁.
		*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK2) = SST_DATA_UNLOCK2;
    	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_SETUP_WRITE;
		*flashPtr = *gBuffer;                                                    //写入数据.
		
		if(sstOpOverDetect((UINT16 *)flashPtr, *gBuffer, 0x1000))             //检测写入是否成功.
		{
			//printf("warning: write flash may failed at:0x%x.\n", (int)flashPtr);
		}
		cLength--;
		flashPtr++;
		gBuffer++;
	}	
	
    flashPtr = flashAddr;
	gBuffer = (UINT16 *)buffer; 
	cLength = length/2;
	for(i=0; i<cLength; i++)			//写入的数据全部校验一次.
	{
	    if(*flashPtr++ != *gBuffer++)
	    {    
	        //printf("Error: write failed in SST39vf160 at 0x%x on verification.\n", (int)flashPtr);
	        return NULL;
	    }
	}
	if(length%2)
	{
	    if((*flashPtr++ & 0x00ff) != (*gBuffer++ & 0x00ff))             /*奇数长度的最后一个字节.*/
	    {
	        //printf("Error: write failed in SST39vf160 at 0x%x on verification.\n", (int)flashPtr);
	        return NULL;	    
	    }
	}
	return (UINT16 *)flashPtr;
}

/********************************************************
函数名称:       sstChipErase()
函数功能:       擦除整个flash芯片.
入口参数:       无.
				
返 回 值:      	OK		 擦除完全正确.
				ERROR    有单元不能正确擦除.
备    注:       
*********************************************************/    
STATUS sstChipErase(void)
{
    int i;
    volatile UINT16 *flashPtr = NULL;
   	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_DATA_UNLOCK1;			//连续解锁.
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK2) = SST_DATA_UNLOCK2;
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_SETUP_ERASE;
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_DATA_UNLOCK1;
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK2) = SST_DATA_UNLOCK2;
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_CHIP_ERASE;			//写入擦除命令.

	flashPtr = (volatile UINT16 *)SST_START_ADDR;			  

	if(sstOpOverDetect((UINT16 *)flashPtr, 0xffff, 0x3000000) != OK)
	{
		//printf("warning: Chip Erase time out!\n");	
	}	
	
	flashPtr = (volatile UINT16 *)SST_START_ADDR;			  
	for(i=0; i<SST_CHIP_HWORD_SIZE; i++,flashPtr++)  					//校验是否全为0xffff.
	{
	   if(*flashPtr != 0xffff)
	   {
	    	//printf("Debug: Erase failed at 0x%x in SST39VF160 on verification.\n", (int)flashPtr);
		    return ERROR;
       }
	}
	return OK;
}    

/********************************************************
函数名称:       sstSectorErase()
函数功能:       擦除指定的flash扇区.
入口参数:       扇区地址.
				
返 回 值:      	OK		 擦除完全正确.
				ERROR    有单元不能正确擦除.
备    注:       
*********************************************************/    
STATUS sstSectorErase(UINT16 *pSector)
{
    int i;
    volatile UINT16 *flashPtr = pSector;
    
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_DATA_UNLOCK1;			//连续解锁.
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK2) = SST_DATA_UNLOCK2;
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_SETUP_ERASE;
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_DATA_UNLOCK1;
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK2) = SST_DATA_UNLOCK2;
	*(volatile UINT16 *)flashPtr = SST_SECTOR_ERASE;			//写入擦除命令.

	if(sstOpOverDetect((UINT16 *)flashPtr, 0xffff, 0x20000) != OK)
	{
		//printf("warning: Chip Erase time out!\n");	
	}	
	
	for(i=0; i<SST_SECTOR_HWORD_SIZE; i++,flashPtr++)  					//校验是否全为0xffff.
	{
	   if(*flashPtr != 0xffff)
	   {
	    	//printf("Debug: Erase failed at 0x%x in SST39VF160 on verification.\n", (int)flashPtr);
		    return ERROR;
       }
	}
	return OK;
} 



