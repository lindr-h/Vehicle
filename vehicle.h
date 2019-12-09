/**
 * ******************************************************************************
 * @file vehicle.h
 * @author Gump
 * @version V0.1
 * @date 2019-12-08
 * @brief 赛车模拟控制程序头文件
 * 
 * ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VEHICLE_H
#define __VEHICLE_H

/* Includes ------------------------------------------------------------------*/
#include "def.h"
#include "consol.h"
#include "S3c2410x.h"
#include "INT.h"
#include "2410lib.h"
#include "lcdlib.h"
#include <math.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
    volatile float position;      /* 赛车位置：用角度表示，取值为0 ~ 360 */
    volatile unsigned char speed; /* 赛车速度：当前最大速度 = 档位(gear) × 30 */
    volatile unsigned char laps;  /* 赛车跑过的圈数 */
    volatile unsigned char gear;  /* 赛车档位: 共四个档 1、2、3、4 */
} Vehicle;

/* Private define ------------------------------------------------------------*/
#define PI 3.14 /* 圆周率Π */

#define BACKGROUND_BUF 0x31000000 /* 背景图像地址 */

#define TRACK_RADIUS 100       /* 赛道半径 */
#define TRACK_CENTRE_X 200.0f  /* 赛道中心横坐标 */
#define TRACK_CENTRE_Y 120.0f  /* 赛道中心纵坐标 */
#define TRACK_WIDTH_HALF 17.0f /* 赛道宽度的一半，实际长度 34 （赛车方块的对角线长度） */
#define TRACK_COLOR 0x000000   /* 赛道颜色 */

#define VEHICLE_WIDTH_HALF 11  /* 赛车宽度的一半，实际长度 23 */
#define VEHICLE_COLOR 0xffff00 /* 赛车颜色 */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void VehicleInit(void);
void VehicleDisplays(void);
void DisplayLED(void);
void LED_TOGGLE(int flag);
void VehicleStatusUpdate(void);
void DrawTrack(void);
void DrawVehicle(float p, COLOR c);
void choose_position(int p, int num, int dot);

/* Exported functions --------------------------------------------------------*/
extern void __irq INT0_Handler(void);
extern void __irq INT1_Handler(void);

#endif /* __VEHICLE_H */

/********************************** END OF FILE *******************************/