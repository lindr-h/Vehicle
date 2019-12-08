#ifndef __GUI_H__
#define __GUI_H__
#include "lcd.h"


#define ADJUST_WIN			0
#define MAIN_WIN			10
#define MAIN_MENU			11
#define GPRS_WIN			12
#define BLUETOOTH_WIN			13
#define USB_WIN				14
#define UART_WIN			15
#define KBD_WIN				16
#define CALL_WIN			17
#define MESSAGE_WIN			18
#define CONFIG_WIN			19
#define GPS_WIN				20
#define CAN_WIN				21
#define STEPMOTOR_WIN			22
#define TIME_CONFIG_WIN			23



#define INVALIDATE_BUTTON		100
#define EXIT_BUTTON			121
#define GPRS_BUTTON			122
#define BLUETOOTH_BUTTON		123
#define	USB_BUTTON			124
#define UART_BUTTON			125
#define IIS_BUTTON			126
#define KBD_BUTTON			127
#define CALL_BUTTON			128
#define MESSAGE_BUTTON			129
#define SEND_BUTTON			130
#define START_BUTTON			131
#define ESC_BUTTON			132
#define GPS_BUTTON			133
#define CONFIG_BUTTON			134

#define CAN_BUTTON			135
#define STEP_MOTOR_BUTTON		136
#define ADJUST_BUTTON		137
#define TIME_BUTTON			138
#define MOTOR_SPEED_UP_BUTTON		139
#define MOTOR_SPEED_DOWN_BUTTON		140
#define MOTOR_ANGLE_UP_BUTTON		141
#define MOTOR_ANGLE_DOWN_BUTTON		142
#define STOP_BUTTON			143
#define SWITCH_BUTTON			144	


typedef struct	
{	
	int			id;
	int			x0;
	int			y0;
	int			text_x;
	int			text_y;
	int			wide;
	int			high;
	COLOR			textcolor;
	COLOR			bgcolor;
	char		text[20];
}BUTTON,*PBUTTON;

extern BUTTON mbuttonlist[MAX];
extern int win_ID;
extern int buttonnumber;
extern int flushenable;

void mainmenu(void);
void changewindows(int x,int y);
int findbuttonid(int x,int y);
void GPRS_win(void);
void drawexitbutton(void);
void fillrectangle(int x0,int y0,int x1,int y1,COLOR mcolor);

#endif