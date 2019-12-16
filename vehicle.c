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
	90, 0, 1, 2}; /* 赛车结构体 */

unsigned char display_flag = 0;

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
	static int led_index;
	static unsigned char led_switch = 0;

	/* 赛车前进 ****************************************************************/
	/* 清除赛车当前位置 */
	DrawVehicle(vehicle.position, 0xffffff);

	/* 赛车位置前进 */
	//if (vehicle.speed < vehicle.gear * 10)
	//{
	vehicle.speed = vehicle.gear * 10;
	/* 后续改为根据加速度递增 */
	//}
	vehicle.position += vehicle.speed;

	/* 圈数增加 ****************************************************************/
	if (vehicle.position > 360)
	{
		vehicle.position -= 360;
		vehicle.laps++;
		if (((vehicle.laps) % 5) == 0)
		{
			display_flag = 1;
		}
	}

	/* 转换灯状态 **************************************************************/
	/* 30 为档位放大系数，闪烁频率与档位呈正相关 */
	if (led_index < (4 - vehicle.gear) * 3)
	{
		led_index++;
		/* LED显示 */
		LED_TOGGLE(led_switch);
	}
	else if (led_index == (4 - vehicle.gear) * 3)
	{
		led_index = 0;

		/* 反转LED状态 */
		led_switch = (led_switch == 1) ? 0 : 1;
	}
}

void VehicleDisplays(void)
{
	/* 绘制赛车 */

	DrawVehicle(vehicle.position, VEHICLE_COLOR);
	/* 显示圈数与速度*/
	if (display_flag != 0)
	{
		DisplayLED();
		display_flag = 0;
	}
}

void DisplayLED(void)
{
	unsigned int i;

	for (i = 0; i < 100; i++)
	{
		/***********************
		 * 数码管前两位显示圈数 *
		 * 第一位：vehicle.laps / 10
		 * 第二位：vehicle.laps % 10
		 **********************/

		choose_position(0, vehicle.laps / 10, 0);
		delay(5);
		choose_position(1, vehicle.laps % 10, 0);
		delay(5);
	}
	choose_position(0, 15, 0);
	//delay(5);
	choose_position(1, 15, 0);

	/***************************
	 * 数码管后两位显示当前速度 * 
	 * 第一位：vehicle.speed / 10
	 * 第二位：vehicle.speed % 10
	 **************************/
	/*
	choose_position(2, vehicle.speed / 10, 0);
	//delay(5);
	choose_position(3, vehicle.speed % 10, 0);
	//delay(5);

*/
}

void LED_TOGGLE(int flag)
{

	if (flag == 1) //led on
	{
		rGPFDAT &= ~(1 << 7);
		rGPFDAT &= ~(1 << 4);
	}
	else //led off
	{
		rGPFDAT |= 1 << 7;
		rGPFDAT |= 1 << 4;
	}
}

/**
 * @brief 绘制赛道
 * 
 */
void DrawTrack(void)
{
	int i, j;

	for (i = 0; i <= 360; i++)
	{
		for (j = TRACK_WIDTH_HALF - 1; j > -TRACK_WIDTH_HALF; j--)
		{
			setpixel(TRACK_CENTRE_X + (TRACK_RADIUS + j) * cos(i * PI / 180.0f),
					 TRACK_CENTRE_Y + (TRACK_RADIUS + j) * sin(i * PI / 180.0f),
					 0xffffff); /* 绘制白色内道 */
		}

		/* 绘制黑色内圈与外圈描边 */
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
			setpixel(TRACK_CENTRE_X + TRACK_RADIUS * cos(p * PI / 180.0f) + i,
					 TRACK_CENTRE_Y + TRACK_RADIUS * sin(p * PI / 180.0f) + j,
					 c);
		}
	}
}

void __irq INT1_Handler(void)
{
	if (vehicle.gear < 4)
	{
		vehicle.gear++;
	}

	ClearInt();
}

void __irq INT0_Handler(void)
{
	if (vehicle.gear > 0)
	{
		vehicle.gear--;
	}

	ClearInt();
}
void choose_position(int p, int num, int dot)
{
	switch (p)
	{
	case 0:
		rGPGDAT &= ~(1 << 2); // 设置数码管的位选 GPG2 = 1  （COM1）
		rGPEDAT &= ~(1 << 9); // 设置数码管的位选 GPE9 = 1（COM2）
		break;
	case 1:
		rGPGDAT |= (1 << 2);  // 设置数码管的位选 GPG2 = 1  （COM1）
		rGPEDAT &= ~(1 << 9); // 设置数码管的位选 GPE9 = 1（COM2）
		break;
	case 2:
		rGPGDAT &= ~(1 << 2); // 设置数码管的位选 GPG2 = 1  （COM1）
		rGPEDAT |= (1 << 9);  // 设置数码管的位选 GPE9 = 1（COM2）
		break;
	case 3:
		rGPGDAT |= (1 << 2); // 设置数码管的位选 GPG2 = 1  （COM1）
		rGPEDAT |= (1 << 9); // 设置数码管的位选 GPE9 = 1（COM2）
		break;
	}

	rGPEDAT = (rGPEDAT & ~(0x3C00)) | (num << 10); // 对应数字(6)
	rGPHDAT = (rGPHDAT & ~(1 << 8)) | (dot << 8);
}
void delay(int time)
{
	int i, j;
	for (i = 0; i < time; i++)
		for (j = 0; j < 1000; j++)
			;
}

/********************************** END OF FILE *******************************/