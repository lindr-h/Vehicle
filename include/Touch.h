#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "def.h"

#define EBS	20

struct POINT
{
	int xData;
	int yData;
};

struct ADJUSTDATA
{
	int flag;
	int adjustdata[4];
};



void adcdly(int number);
void WaitForStylusUp(void);
void tp_rewaite_int(void);
int adc_x_position(void);
int adc_y_position(void);
void __irq TouchPanel_LISR(void);
void INT_ADC_Enable(int flag);
//void ADC_Entry(void);
//void Ts_Auto(void);
int compare(int num1,int num2,int pt);
void DrawAcross(int x,int y,COLOR mcolor);
void LineDrawAcross(int x0,int y0,int x1,int y1,COLOR mcolor);
void RestoreAcross(int x,int y);
void ReadAdjustData(void);


void Adjust(void);
#endif
