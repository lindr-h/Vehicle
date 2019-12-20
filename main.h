/**
 * ******************************************************************************
 * @file main.h
 * @author Gump, LDR
 * @version V0.1
 * @date 2019-12-08
 * @brief 汽车模拟控制程序主函数头文件
 * 
 * ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "def.h"
#include "consol.h"
#include "S3c2410x.h"
#include "INT.h"
#include "2410lib.h"
#include "lcdlib.h"
#include <math.h>

/* Private function prototypes -----------------------------------------------*/
void SystemInit(void);
void PeripheralInit(void);
void ExitInit(void);
void ShowCover(void);
void EINT0_Enable(int flag);
void EINT2_Enable(int flag);

/* Private functions ---------------------------------------------------------*/