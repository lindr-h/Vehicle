/**
 * ******************************************************************************
 * @file main.c
 * @author Gump, LDR
 * @version V0.1
 * @date 2019-12-08
 * @brief 赛车模拟控制程序主函数源文件
 * 
 * ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "vehicle.h"

/* Private function ----------------------------------------------------------*/

/**
 * @brief 程序主函数
 * 
 */
int main(void)
{

	/* 系统初始化 */
	SystemInit();
	PeripheralInit();
	ExitInit();

	/* 显示小组成员信息 */
	ShowCover();

	/* 赛车模拟控制主程序 */
	VehicleInit();
	while (1)
	{
		VehicleStatusUpdate();
		VehicleDisplays();
	}
}

/**
 * @brief 系统初始化
 * 
 */
void SystemInit(void)
{

	ChangeClockDivider(1, 1);		 // 1:2:4
	ChangeMPllValue(0xa1, 0x3, 0x1); // FCLK=202.8MHz
}

/**
 * @brief 外设初始化
 * 
 */
void PeripheralInit(void)
{

	Port_Init();
	LCD_Init();
	clrsrc(0xffff);  // 白色清屏
	LcdHorizontal(); // 横屏
}

/**
 * @brief 中断初始化
 * 
 */
void ExitInit(void)
{

	__vInitHaltHandlers();

	EINT0_Enable(True);
	ISRVector[0] = INT0_Handler; // 中断0处理函数

	EINT2_Enable(True);
	ISRVector[2] = INT2_Handler; // 中断2处理函数

	/* LED 配置 */
	rGPFCON |= ((1 << 8) | (1 << 14)); // set GPF4,GPF7 为输出管脚

	/* 数码管配置 */
	rGPGCON = (rGPGCON & ~(3 << 4)) | (1 << 4); // 设置 GPG2 = OUTPUT
	rGPGUP |= 1 << 2;							// 共16位，将GPG2引脚对应的2号位置设置为拉电流

	rGPECON = (rGPECON & ~(3 << 18)) | (1 << 18); // 设置 GPE9 = OUTPUT
	rGPEUP |= 1 << 9;							  // 共16位，将GPE9引脚对应的9号位置设置为拉电流

	rGPECON = (rGPECON & ~(0xFF00000)) | (0x5500000);		 // 设置 GPE[10 ~ 13] = OUTPUT
	rGPEUP |= (1 << 10) | (1 << 11) | (1 << 12) | (1 << 13); // 将GPE10-13引脚设置为拉电流

	rGPHCON = (rGPHCON & ~(3 << 16)) | (1 << 16); // 设置点,不亮(GPH8 = OUTPUT)
	rGPHUP |= 1 << 8;
}

/**
 * @brief 在LCD上显示成员信息，持续一段时间
 * 
 */
void ShowCover(void) {

	unsigned char *ptr = (unsigned char *)COVER_BUF;
	ShowImage(ptr, 0, 0, 1);
	
	delay(5000);
}

/**
 * @brief 中断0使能函数
 * 
 * @param flag 使能标志
 */
void EINT0_Enable(int flag)
{
	int temp;
	if (flag == True)
	{
		temp = rGPFCON;
		rGPFCON = temp | 0x02;
		temp = rGPFDAT;
		rGPFDAT = temp | 0x01;
		rINTMSK &= ~(BIT_EINT0); // 掩盖寄存器掩盖中断0
	}
	else
	{
		rINTMSK &= BIT_EINT0;
	}
}

/**
 * @brief 中断2使能函数
 * 
 * @param flag 使能标志
 */
void EINT2_Enable(int flag)
{
	int temp;
	if (flag == True)
	{
		temp = rGPFCON;
		rGPFCON = temp | 0x20;
		temp = rGPFDAT;
		rGPFDAT = temp | 0x01;
		rINTMSK &= ~(BIT_EINT2); // 掩盖寄存器掩盖中断0
	}
	else
	{
		rINTMSK &= BIT_EINT2;
	}
}

/********************************** END OF FILE *******************************/