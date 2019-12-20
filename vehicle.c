/**
 * ******************************************************************************
 * @file vehicle.h
 * @author Gump, LDR
 * @version V0.1
 * @date 2019-12-08
 * @brief 赛车模拟控制程序源文件
 * 
 * ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "vehicle.h"

/* Private variables ---------------------------------------------------------*/
Vehicle vehicle = {
	90, 0, 1, 2}; /*!< 赛车结构体 */

char speed[10];					/*!< LCD 左上角显示的当前速度 */
unsigned char display_flag = 0; /*!< 显示控制标志 */

/* Private functions ---------------------------------------------------------*/

/**
 * @brief 赛车程序初始化函数
 * 
 */
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

/**
 * @brief 更新赛车数据
 * 
 * @todo 根据为赛车增加加速度属性值，使得赛车速度切换更加真实
 */
void VehicleStatusUpdate(void)
{
	static int led_counter;				 /*!< LED 闪烁计数器 */
	static unsigned char led_status = 0; /*!< LED 当前状态 */

	/* 赛车前进 ****************************************************************/
	/* 清除赛车当前位置 */
	DrawVehicle(vehicle.position, 0xffffff);

	// if (vehicle.speed < vehicle.gear * 10) /* 后续改为根据加速度递增 */
	/* “档位”映射为“速度” */
	vehicle.speed = vehicle.gear * 10;

	/* “速度”成员变量格式化为字符串字面量 */
	sprintf(speed, "Speed: %2d", vehicle.speed);

	/* 赛车位置前进 */
	vehicle.position += vehicle.speed;

	/* 圈数增加 ****************************************************************/
	if (vehicle.position > 360)
	{
		vehicle.position -= 360;
		vehicle.laps++;
		/* 每五圈就在数码管上显示当前圈数 */
		if (((vehicle.laps) % 5) == 0)
		{
			display_flag = 1;
		}
		/* 只显示两位圈数 */
		if (vehicle.laps >= 100)
		{
			vehicle.laps -= 100;
		}
	}

	/* 转换灯状态 **************************************************************/
	/* 计数器循环计数 */
	/* 数字 3 为档位放大系数，闪烁频率与档位呈正相关 */
	if (led_counter < (4 - vehicle.gear) * 3)
	{
		led_counter++;
		/* LED显示 */
		LED_Display(led_status);
	} /* 计数完成时 */
	else if (led_counter >= (4 - vehicle.gear) * 3)
	{
		/* 计数器清零 */
		led_counter = 0;

		/* 反转LED状态 */
		led_status = (led_status == 1) ? 0 : 1;
	}
}

/**
 * @brief 显示赛车状态
 * 
 */
void VehicleDisplays(void)
{
	/* 绘制赛车 */
	DrawVehicle(vehicle.position, VEHICLE_COLOR);

	DrawRect();
	drawascii168(12, 220, speed, 1, 1, 0xffffff);

	/* 显示圈数与速度*/
	if (display_flag != 0)
	{
		DisplaySMG();
		display_flag = 0;
	}
}

/**
 * @brief 数码管显示函数 在左边两位显示当前已走圈数
 * 
 */
void DisplaySMG(void)
{
	unsigned int i;

	for (i = 0; i < 100; i++) /* 显示一段时间 */
	{
		/* 数码管前两位显示圈数 */
		DisplayOneSMG(0, vehicle.laps / 10, 0);
		delay(5);
		DisplayOneSMG(1, vehicle.laps % 10, 0);
		delay(5);
	}

	/* 显示完后清除数码管数据 */
	DisplayOneSMG(0, 15, 0);
	DisplayOneSMG(1, 15, 0);
}

/**
 * @brief 数码管显示函数
 * 
 * @param p 数字在数码管中的位置
 * 		@arg 0..3 从右到左依次为 0 ~ 3 位
 * @param num 要显示的数字
 * @param dot 是否显示小数点
 * 		@arg 0 不显示小数点
 * 		@arg 1 显示小数点
 */
void DisplayOneSMG(int p, int num, int dot)
{
	status(p)
	{
	case 0:
		rGPGDAT &= ~(1 << 2); /* 设置数码管的位选 GPG2 = 0 (COM1) */
		rGPEDAT &= ~(1 << 9); /* 设置数码管的位选 GPE9 = 0 (COM2) */
		break;
	case 1:
		rGPGDAT |= (1 << 2);  /* 设置数码管的位选 GPG2 = 1 (COM1) */
		rGPEDAT &= ~(1 << 9); /* 设置数码管的位选 GPE9 = 0 (COM2) */
		break;
	case 2:
		rGPGDAT &= ~(1 << 2); /* 设置数码管的位选 GPG2 = 0 (COM1) */
		rGPEDAT |= (1 << 9);  /* 设置数码管的位选 GPE9 = 1 (COM2) */
		break;
	case 3:
		rGPGDAT |= (1 << 2); /* 设置数码管的位选 GPG2 = 1 (COM1) */
		rGPEDAT |= (1 << 9); /* 设置数码管的位选 GPE9 = 1 (COM2) */
		break;
	}

	rGPEDAT = (rGPEDAT & ~(0x3C00)) | (num << 10); /* 对应数字 6 */
	rGPHDAT = (rGPHDAT & ~(1 << 8)) | (dot << 8);
}

/**
 * @brief LED 显示函数
 * 
 * @param flag 控制标志
 */
void DisplayLED(int flag)
{

	if (flag == 1) /* led on */
	{
		rGPFDAT &= ~(1 << 7);
		rGPFDAT &= ~(1 << 4);
	}
	else /* led off */
	{
		rGPFDAT |= 1 << 7;
		rGPFDAT |= 1 << 4;
	}
}

/**
 * @brief 绘制圆形赛道
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

		/* 绘制黑色赛道描边 */
		/* 用弧度表示角度，1度= Π/180（rad) */
		setpixel(TRACK_CENTRE_X + (TRACK_RADIUS + TRACK_WIDTH_HALF) * cos(i * PI / 180.0f),
				 TRACK_CENTRE_Y + (TRACK_RADIUS + TRACK_WIDTH_HALF) * sin(i * PI / 180.0f),
				 TRACK_COLOR); /* 绘制外描边 */
		setpixel(TRACK_CENTRE_X + (TRACK_RADIUS - TRACK_WIDTH_HALF) * cos(i * PI / 180.0f),
				 TRACK_CENTRE_Y + (TRACK_RADIUS - TRACK_WIDTH_HALF) * sin(i * PI / 180.0f),
				 TRACK_COLOR); /* 绘制内描边 */
	}
}

/**
 * @brief 绘制赛车，即一个小矩形
 * 
 * @param p 赛车当前所处在赛道的位置
 * @param c 赛车颜色
 */
void DrawVehicle(float p, COLOR c)
{
	int i, j;

	/* 以矩形的几何中心为定位点，从左上角到右下角绘制矩形 */
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

/**
 * @brief 绘制蓝色矩形方块
 * 
 */
void DrawRect(void)
{

	int i, j;

	for (i = 0; i < SPEED_W; i++)
	{
		for (j = 0; j < SPEED_H; j++)
		{
			setpixel(SPEED_X + i,
					 SPEED_Y + j,
					 0x0000ff);
		}
	}
}

/**
 * @brief 按键2中断处理函数 每按一次“档位”成员变量自增一
 * 
 */
void __irq INT2_Handler(void)
{
	if (vehicle.gear < 3)
	{
		vehicle.gear++;
	}

	ClearInt();
}

/**
 * @brief 按键1中断处理函数 每按一次“档位”成员变量自减一
 * 
 */
void __irq INT0_Handler(void)
{
	if (vehicle.gear > 0)
	{
		vehicle.gear--;
	}

	ClearInt();
}

/**
 * @brief 简单的延时函数
 * 
 * @param time 延时次数
 */
void delay(int time)
{
	int i, j;
	for (i = 0; i < time; i++)
		for (j = 0; j < 1000; j++)
			;
}

/********************************** END OF FILE *******************************/