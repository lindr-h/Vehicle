#include <string.h>
#include <stdarg.h>
#include "2410addr.h"
#include "def.h"
#include "touch.h"
#include "lcdlib.h"
#include "IIC.h"
//#include "GUI.h"
//#include "lcd.h"
//#define ADCPRS 39

//void WIN_Adjust(int mode);
//void WIN_Drawing(int x0, int y0, int x1, int y1);
extern COLOR getpixel(int x,int y);
void INT_ADC_Enable(int flag);
//int stylusup;
//int win_ID;

int ADJUST[4];
struct POINT point[5];
int AdjustID;					// 准星点ID号
int IsAdjusted;					// 判断校正是否成功
struct ADJUSTDATA adjustData;	// 保存校正数据
short restor_image_data[135];	// 保存准星,用作恢复现场

char *NCL_itoa(int value, char *string)
{
	int i, d;
	int flag = 0;
	char *ptr = string;


	if(!value)
	{
		*ptr++ = 0x30;
		*ptr = 0;

		return string;
	}

	/* if this is a negative value insert the minus sign. */
	if(value < 0)
	{
		*ptr++ = '-';

		/* Make the value positive. */
		value *= -1;
	}

	for(i = 10000; i > 0; i /= 10)
	{
		d = value / i;

		if(d || flag)
        {
			*ptr++ = (char)(d + 0x30);
			value -= (d * i);
			flag = 1;
		}
	}

	/* Null terminate the string. */
	*ptr = 0;

	return string;
} /* NCL_itoa */



void adcdly(int Number)
{   
    int i,t;
    
    // delaying the time
    for(i = 0; i < Number; i++)
    	for(t = 0; t < 0x100; t++);
 }

void tp_rewaite_int(void)
{
	//define the pointer varible point to the register
	rADCTSC = 0xd3;	//confingure the register ADCTSC renable the touchpanel interrupt
}




int adc_xData_position(int amount)
{   
	//define the pointer varible and point to the register
	int i;
	int adc_xdata = 0;//x_data;
	//char str[10];	
	for (i=0; i<amount; i++)
	{
		/*confingure the ADCTSC register and begin the X axis covision*/
		rADCTSC = 0x9E;
		rADCCON = 0x7FE9;
		while (!(rADCCON & 0x8000));  /*if the covision is end?*/
		//adcdly(1);                     /*delay little time for more nicety */
		adc_xdata += (rADCDAT1)&0x3ff;  /*get the value from register ADCADT0*/
	}
	adc_xdata /= amount;
	//NCL_itoa(adc_xdata,str);
	return adc_xdata;
 }   

int adc_yData_position(int amount)
{
    /*define the pointer varible and point to the register*/
    int i;
    int adc_ydata =0;//y_data;
    //char str[10];
  
    for (i=0; i<amount; i++)
    {
    	rADCTSC=0x6C;     /*confingure the ADCTSC register and begin the Y axis covision*/
    	rADCCON=0x7FF9;
    	while (!(rADCCON & 0x8000)); /*if the covision is end?*/
    	//adcdly(1);                    /*delay little time for more nicety */
    	adc_ydata += (rADCDAT0)&0x3ff; /*get the value from register ADCADT0*/
    }
    
    adc_ydata /= amount;
        
    //NCL_itoa(adc_ydata,str);	
    return adc_ydata;
} 
 
int adc_y_position()
{
	int temp;
	int yData ;
#ifdef STN
	yData = adc_xData_position(5);
	temp =( yData - point[0].yData)*400/(point[2].yData-point[0].yData) +40;
#else	
	if (mGuiOrientation == GUI_HORIZONTAL)
	{
		yData = adc_yData_position(5);
		temp =( yData - ADJUST[1]) * 160 / (ADJUST[3] - ADJUST[1]) + 40;
		//temp =( yData - point[1].yData) * 160 / (point[3].yData - point[1].yData) + 40;
	}
	else
	{
		yData = adc_xData_position(5);
		temp = (yData - ADJUST[2]) * 240 / (ADJUST[0] - ADJUST[2]) + 40;
		//temp = (yData - point[2].xData) * 240 / (point[0].xData - point[2].xData) + 40;
	}
#endif	
	return temp;
} 
 
 
int adc_x_position()
{
	int temp;
	int xData;

#ifdef STN
	xData = adc_yData_position(5);
	temp =( xData - point[0].xData)*560/(point[2].xData-point[0].xData) +40;
#else	
	if (mGuiOrientation == GUI_HORIZONTAL)
	{
		xData = adc_xData_position(5);
		temp =( xData - ADJUST[0]) * 240 / (ADJUST[2] - ADJUST[0]) + 40;
		//temp =( xData - point[0].xData) * 240 / (point[2].xData - point[0].xData) + 40;
	}
	else
	{
		xData = adc_yData_position(5);

		temp =( xData - ADJUST[1]) * 160 / (ADJUST[3] - ADJUST[1]) + 40;
		//temp =( xData - point[1].yData) * 160 / (point[3].yData - point[1].yData) + 40;
	}
#endif	
	return temp;
}

/*
void __irq TouchPanel_LISR(void)
{
	
	volatile int xPosition;
	volatile int yPosition;
	
	if(IISfinished ==0 );
	else if(win_ID == ADJUST_WIN) 
			Adjust();
	       
	else	
	{
			yPosition = adc_y_position();
			xPosition = adc_x_position();
	}
	tp_rewaite_int();	
	INT_ADC_Enable(TRUE);
}
*/

void WaitForStylusUp(void)
{
	int i;
	INT_ADC_Enable(FALSE);
	tp_rewaite_int();
	for(i=1;i<10000;i++);
	while(!(rADCDAT0&0x8000));
	INT_ADC_Enable(TRUE);

}	
	

void LineDrawAcross(int x0,int y0,int x1,int y1,COLOR mcolor)
{
	int i,x,y;
	if( x0 == x1)
	{	
		if(y0 < y1)
		{
		    for(y=y0;y<y1;y++)
		    {
		       	RestoreAcross(x0,y);
				for(i=0;i<1000;i++);
				DrawAcross(x0,y+1,mcolor);
			}
		}
		else
		{
			for(y=y0;y>y1;y--)
		    {
		       	RestoreAcross(x0,y);
				for(i=0;i<1000;i++);
				DrawAcross(x0,y-1,mcolor);
			}
		}
	}
	
	if( x0 < x1 )
	{
		if(y0 < y1)
		{
			for(x=x0;x<x1;x++)
			{
				y=(int)(  ( (float)(x-x0) ) * (y1-y0)/(x1-x0) + y0 );
				RestoreAcross(x,y);
				for(i=0;i<1000;i++);
				y=(int)(  ( (float)(x+1-x0) ) * (y1-y0)/(x1-x0) + y0 );
				DrawAcross(x+1,y,mcolor);
			}
		}
		else
		{
			for(x=x0;x<x1;x++)
			{
				y=(int)( y0 - ( (float)(x-x0) ) * (y0-y1)/(x1-x0)  );
				RestoreAcross(x,y);
				for(i=0;i<1000;i++);
				y=(int)( y0 - ((float)(x+1-x0)) * (y0-y1)/(x1-x0)  );
				DrawAcross(x+1,y,mcolor);
			}
		}
	}
	
	if( x0 > x1 )
	{
		if(y0 < y1)
		{
			for(x=x0;x>x1;x--)
			{
				y=(int)(  ( (float)(x-x0) ) * (y0-y1)/(x0-x1) + y0 );
				RestoreAcross(x,y);
				for(i=0;i<1000;i++);
				y=(int)(  ( (float)(x-1-x0) ) * (y0-y1)/(x0-x1) + y0 );
				DrawAcross(x-1,y,mcolor);
			}
		}
		else
		{
			for(x=x0;x>x1;x--)
			{
				y=(int)( y0 - ( (float)(x-x0) ) * (y0-y1)/(x1-x0)  );
				RestoreAcross(x,y);
				for(i=0;i<1000;i++);
				y=(int)( y0 - ((float)(x-1-x0)) * (y0-y1)/(x1-x0)  );
				DrawAcross(x-1,y,mcolor);
			}
		}
		
	}
}

void DrawAcross(int x,int y,COLOR mcolor)
{
	int i;
	int k=0;
	for(i=x-10;i<=x+11;i++)				//保存像素
	{
		restor_image_data[k++] = getpixel(i,y);
		restor_image_data[k++] = getpixel(i,y+1);
	}
	for(i=y-10;i<=y+11;i++)
	{
		restor_image_data[k++] = getpixel(x,i);	
		restor_image_data[k++] = getpixel(x+1,i);		
	}
	line(x-10,y,x-1,y,mcolor);
	line(x-10,y+1,x-1,y+1,mcolor);
	line(x+2,y,x+11,y,mcolor);
	line(x+2,y+1,x+11,y+1,mcolor);	
	
	
	line(x,y-10,x,y-1,mcolor);
	line(x+1,y-10,x+1,y-1,mcolor);
	line(x,y+2,x,y+11,mcolor);
	line(x+1,y+2,x+1,y+11,mcolor);	

}

void RestoreAcross(int x,int y)
{
	int i;
	int k=0;
	for(i=x-10;i<=x+11;i++)				//恢复像素
	{
		setpixel(i,y,restor_image_data[k++] );
		setpixel(i,y+1,restor_image_data[k++] );
	}
	for(i=y-10;i<=y+11;i++)
	{
		setpixel(x,i,restor_image_data[k++] );
		setpixel(x+1,i,restor_image_data[k++] );
	}
}

void InitPoint()
{
	int i;
	for(i=0;i<4;i++)
	{
		point[i].xData = 0;
		point[i].yData = 0;
	}
}	


int compare(int num1,int num2,int pt)
{
	int temp;
	if(num1 > num2) temp = num1 - num2;
		else temp = num2 - num1;
	if (temp < pt)
		return 1;
	else    return 0;
}

void INT_ADC_Enable(int flag)
{

	if(flag == FALSE)
	{   		
		rINTSUBMSK |= (1<<10);
		rINTMSK |= 0x80000000;
	}
	else 
	{
		rADCTSC = 0xD3;
   		rADCDLY = 0xc350;
		rINTSUBMSK &= ~(BIT_SUB_TC);
		rINTMSK &= ~(BIT_ADC);
	}
}

void ReadAdjustData(void)
{
	unsigned int k;

	for(k=0;k<sizeof(struct ADJUSTDATA);k++)
	{
		IIC_read(k,(unsigned char*)(&adjustData) + k);
	}
	
	if (adjustData.flag ==1)
	{
	
		ADJUST[0] = adjustData.adjustdata[0];
		ADJUST[1] = adjustData.adjustdata[1];
		ADJUST[2] = adjustData.adjustdata[2];
		ADJUST[3] = adjustData.adjustdata[3];
	}
}