/**
 * ******************************************************************************
 * @file main.c
 * @author Gump
 * @version V0.1
 * @date 2019-12-08
 * @brief 汽车模拟控制程序主函数源文件
 * 
 * ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "vehicle.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

int main(void) {

	SystemInit();
	PeripheralInit();

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
void SystemInit(void) {

	ChangeClockDivider(1, 1);		 // 1:2:4
	ChangeMPllValue(0xa1, 0x3, 0x1); // FCLK=202.8MHz

}

/**
 * @brief 外设初始化
 * 
 */
void PeripheralInit(void) {

	Port_Init();
	LCD_Init();
	clrsrc(0xffff);  //白色清屏
	LcdHorizontal(); //横屏
	
}