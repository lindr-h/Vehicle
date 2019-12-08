/**
 * ******************************************************************************
 * @file vehicle.h
 * @author Gump
 * @version V0.1
 * @date 2019-12-08
 * @brief 汽车模拟控制程序源文件
 * 
 * ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "vehicle.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void VehicleInit(void)
{

    /* 绘制跑道背景图 */
    unsigned char *ptr = (unsigned char *)IMAGEBUF;
    ShowImage(ptr, 0, 0, 1);

    /* 绘制跑道 */
    DrawTrack(, , ); //外圈
    DrawTrack(, , ); //内圈

    /* 绘制汽车 */
}

/**
 * @brief 绘制赛道
 * 
 * @param X 圆心横坐标
 * @param Y 圆心纵坐标
 * @param R 圆半径
 */
void DrawTrack(void)
{
    int i;
    for (i = 0; i <= 360; i++)
    {
        /* 用弧度表示角度，1度= Π/180（rad) */
        setpixel(TRACK_CENTRE_X + (TRACK_RADIUS + TRACK_WIDTH_HALF) * cos(i * PI / 180),
                 TRACK_CENTRE_Y + (TRACK_RADIUS + TRACK_WIDTH_HALF) * sin(i * PI / 180),
                 TRACK_COLOR); /* 绘制外圈 */
        setpixel(TRACK_CENTRE_X + (TRACK_RADIUS - TRACK_WIDTH_HALF) * cos(i * PI / 180),
                 TRACK_CENTRE_Y + (TRACK_RADIUS - TRACK_WIDTH_HALF) * sin(i * PI / 180),
                 TRACK_COLOR); /* 绘制内圈 */
    }
}

/**
 * @brief 绘制赛车，即一个小矩形
 * 
 * @param x 赛车横坐标
 * @param y 赛车纵坐标
 * @param mcolor 
 */
void DrawVehicle(int x, int y, COLOR mcolor)
{
    int i;
    for (i = 0; i < 25; i++)
    {
        int j;
        for (j = 0; j < 25; j++)
            setpixel(x + j, y + i, mcolor);
    }
}
