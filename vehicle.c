/**
 * ******************************************************************************
 * @file vehicle.h
 * @author Gump
 * @version V0.1
 * @date 2019-12-08
 * @brief 赛车模拟控制程序源文件
 * 
 * ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "vehicle.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
Vehicle vehicle = {
	.position = 90,
	.speed = 0,
	.laps = 0,
	.gear = 0}; /* 赛车结构体 */

/* Private functions ---------------------------------------------------------*/
void VehicleInit(void)
{
	/* 绘制跑道背景图 */
	unsigned char *ptr = (unsigned char *)BACKGROUND_BUF;
	ShowImage(ptr, 0, 0, 1);

	/* 绘制跑道 */
	DrawTrack();

	/* 绘制赛车 */
	DrawVehicle(90, VEHICLE_COLOR);
}

void VehicleStatusUpdate(void)
{
	static int position_index;
	static int led_index;

	/* 赛车前进 ****************************************************************/
	/* 清除赛车当前位置 */
	DrawVehicle(vehicle.position, 0xffffff);

	/* 赛车位置前进 */
	if (vehicle.speed < vehicle.gear * 30)
	{
		vehicle.speed = vehicle.gear * 30;
		/* 后续改为根据加速度递增 */
	}
	vehicle.position += vehicle.speed;

	/* 圈数增加 ****************************************************************/
	if (vehicle.position > 360)
	{
		vehicle.position -= 360;
		vehicle.laps++;
	}

	/* 转换灯状态 **************************************************************/
	/* 30 为档位放大系数，闪烁频率与档位呈正相关 */
	if (led_index < vehicle.gear * 30)
	{
		led_index++;
	}
	else if (led_index == 0)
	{
		led_index = 0;

		/****************
		 * LED_TOGGLE() * 
		 ****************/
	}
}

void VehicleDisplays(void)
{
	/* 绘制赛车 */
	DrawVehicle(vehicle.position, VEHICLE_COLOR);

	/* 显示圈数与速度 */
	DisplayLED();
}

void DisplayLED(void)
{
	/***********************
	 * 数码管前两位显示圈数 *
	 * 第一位：vehicle.laps / 10
	 * 第二位：vehicle.laps % 10
	 **********************/

	/***************************
	 * 数码管后两位显示当前速度 * 
	 * 第一位：vehicle.speed / 10
	 * 第二位：vehicle.speed % 10
	 **************************/
}

/**
 * @brief 绘制赛道
 * 
 */
void DrawTrack(void)
{
	int i;

	for (i = 0; i <= 360; i++)
	{
		/* 用弧度表示角度，1度= Π/180（rad) */
		setpixel(TRACK_CENTRE_X + (TRACK_RADIUS + TRACK_WIDTH_HALF) * cos(i * PI / 180.0f),
				 TRACK_CENTRE_Y + (TRACK_RADIUS + TRACK_WIDTH_HALF) * sin(i * PI / 180.0f),
				 TRACK_COLOR); /* 绘制外圈 */
		setpixel(TRACK_CENTRE_X + (TRACK_RADIUS - TRACK_WIDTH_HALF) * cos(i * PI / 180.0f),
				 TRACK_CENTRE_Y + (TRACK_RADIUS - TRACK_WIDTH_HALF) * sin(i * PI / 180.0f),
				 TRACK_COLOR); /* 绘制内圈 */
	}
}

/**
 * @brief 绘制赛车，即一个小矩形
 * 
 * @param p 赛车当前所处在赛道的位置
 */
void DrawVehicle(float p, COLOR c)
{
	int i, j;

	for (i = -VEHICLE_WIDTH_HALF; i < VEHICLE_WIDTH_HALF; i++)
	{
		for (j = -VEHICLE_WIDTH_HALF; j < VEHICLE_WIDTH_HALF; j++)
		{
			setpixel(TRACK_CENTRE_X + (TRACK_RADIUS + TRACK_WIDTH_HALF) * cos(p * PI / 180.0f) + i,
					 TRACK_CENTRE_X + (TRACK_RADIUS + TRACK_WIDTH_HALF) * sin(p * PI / 180.0f) + j,
					 c);
		}
	}
}

void INT1_Handler(void)
{
	vehicle.gear++;
}

void INT0_Handler(void)
{
	if (vehicle.gear > 0)
	{
		vehicle.gear--;
	}
}

/********************************** END OF FILE *******************************/