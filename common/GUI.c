#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gui.h"
#include "lcd.h"
#include "key.h"
#include "2410iis.h"
#include "at.h"
#include "IIC_def.h"
#include "StepMotor.h"
#include "rtc.h"

extern char phone_num[30];
extern int  phone_index;
extern int gTimeIndex;
extern char gTimestr[20];
extern int a[4];
extern int gAngle;			//步进电机的角度
extern int gfreq;			//步进电机的频率
extern int gDirectoryCtrl;  //步进电机的角度
TIME time;
void motorctrl(int id);

char *strcopy(register char *s1, const register char *s2)
{
    register char *p = s1;

    while ((*s1++ = *s2++) != 0);

    return p;
}

void GUI_cls(void)
{	
	
	int i;
	while(i++<10000);
	clrsrc(rgb(0,0,255));
	for(i=0;i<320;i++)
	{
	 setpixel(i,239,rgb(0,255,0));
	 setpixel(i+1,239,rgb(0,255,0));
	 setpixel(i+1,238,rgb(0,255,0));
	 setpixel(i,238,rgb(0,255,0));
	 setpixel(i,0,rgb(0,255,0));
	 setpixel(i+1,0,rgb(0,255,0));
	 setpixel(i+1,1,rgb(0,255,0));
	 setpixel(i,1,rgb(0,255,0));

	 }
	 
	for(i=0;i<320;i++)
	{
	 setpixel(0,i,rgb(0,255,0));
	 setpixel(1,i,rgb(0,255,0));
	 setpixel(1,i+1,rgb(0,255,0));
	 setpixel(0,i+1,rgb(0,255,0));
	 setpixel(319,i,rgb(0,255,0));
	 setpixel(318,i,rgb(0,255,0));
	 setpixel(319,i+1,rgb(0,255,0));
	 setpixel(318,i+1,rgb(0,255,0));

	 }
	 
}



void fillreverserectangle(int x0,int y0,int x1,int y1)
{
	int x;
	int y;
	COLOR mcolor;
	for(x=x0;x<=x1;x++)
	   for(y=y0;y<=y1;y++)
	   {
	   	mcolor = getpixel(x,y);
	    setpixel(x,y,~mcolor);
	   }
}

void drawbutton(PBUTTON pbutton)
{
	fillrectangle(pbutton->x0,pbutton->y0, pbutton->x0+pbutton->wide,pbutton->y0+pbutton->high,pbutton->bgcolor);
	line(pbutton->x0,pbutton->y0,pbutton->x0+pbutton->wide,pbutton->y0,pbutton->textcolor);
	line(pbutton->x0+pbutton->wide,pbutton->y0,pbutton->x0+pbutton->wide,pbutton->y0+pbutton->high,pbutton->textcolor);
	line(pbutton->x0,pbutton->y0+1,pbutton->x0+pbutton->wide,pbutton->y0+1,pbutton->textcolor);
	
	drawascii168(pbutton->x0+pbutton->text_x,pbutton->y0+pbutton->text_y,pbutton->text,pbutton->textcolor);
}

void clearbuttonlist(void)
{
	int i;
	for(i=0;i< buttonnumber;i++)
	{
		mbuttonlist[i].id = INVALIDATE_BUTTON;
		mbuttonlist[i].wide = 0;
		mbuttonlist[i].high = 0;
	}
}
		

void mainmenu(void)
{
	GUI_cls();
	buttonnumber = 10;	
	drawtext(120,230,"演示主窗口",1.2,3,rgb(255,255,0));
//	drawtext(121,231,"演示主窗口",1.2,3,rgb(255,255,0));
	
	mbuttonlist[0].id = IIS_BUTTON;
	mbuttonlist[0].x0 = 60;
	mbuttonlist[0].y0 = 170;
	mbuttonlist[0].wide =80;
	mbuttonlist[0].high = 30;
	mbuttonlist[0].text_x = 30;
	mbuttonlist[0].text_y = 20;
	mbuttonlist[0].textcolor = rgb(255,255,0);
	mbuttonlist[0].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[0].text,"IIS");
	drawbutton(&mbuttonlist[0]);
	
	mbuttonlist[1].id = CONFIG_BUTTON;
	mbuttonlist[1].x0 = 180;
	mbuttonlist[1].y0 = 170;
	mbuttonlist[1].wide =80;
	mbuttonlist[1].high = 30;
	mbuttonlist[1].text_x = 20;
	mbuttonlist[1].text_y = 20;
	mbuttonlist[1].textcolor = rgb(255,255,0);
	mbuttonlist[1].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[1].text,"Config");
	drawbutton(&mbuttonlist[1]);


	mbuttonlist[2].id = USB_BUTTON;
	mbuttonlist[2].x0 = 60;
	mbuttonlist[2].y0 = 130;
	mbuttonlist[2].wide =80;
	mbuttonlist[2].high = 30;
	mbuttonlist[2].text_x = 30;
	mbuttonlist[2].text_y = 20;
	mbuttonlist[2].textcolor = rgb(255,255,0);
	mbuttonlist[2].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[2].text,"USB");
	drawbutton(&mbuttonlist[2]);


	mbuttonlist[3].id = KBD_BUTTON;
	mbuttonlist[3].x0 = 180;
	mbuttonlist[3].y0 = 130;
	mbuttonlist[3].wide =80;
	mbuttonlist[3].high = 30;
	mbuttonlist[3].text_x = 30;
	mbuttonlist[3].text_y = 20;
	mbuttonlist[3].textcolor = rgb(255,255,0);
	mbuttonlist[3].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[3].text,"KBD");
	drawbutton(&mbuttonlist[3]);
	
	mbuttonlist[4].id = GPS_BUTTON;
	mbuttonlist[4].x0 = 60;
	mbuttonlist[4].y0 = 90;
	mbuttonlist[4].wide =80;
	mbuttonlist[4].high = 30;
	mbuttonlist[4].text_x = 30;
	mbuttonlist[4].text_y = 20;
	mbuttonlist[4].textcolor = rgb(255,255,0);
	mbuttonlist[4].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[4].text,"GPS");
	drawbutton(&mbuttonlist[4]);

	mbuttonlist[5].id = CAN_BUTTON;
	mbuttonlist[5].x0 = 180;
	mbuttonlist[5].y0 = 90;
	mbuttonlist[5].wide =80;
	mbuttonlist[5].high = 30;
	mbuttonlist[5].text_x = 30;
	mbuttonlist[5].text_y = 20;
	mbuttonlist[5].textcolor = rgb(255,255,0);
	mbuttonlist[5].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[5].text,"CAN");
	drawbutton(&mbuttonlist[5]);

	mbuttonlist[6].id = GPRS_BUTTON;
	mbuttonlist[6].x0 = 180;
	mbuttonlist[6].y0 = 50;
	mbuttonlist[6].wide =80;
	mbuttonlist[6].high = 30;
	mbuttonlist[6].text_x = 25;
	mbuttonlist[6].text_y = 20;
	mbuttonlist[6].textcolor = rgb(255,255,0);
	mbuttonlist[6].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[6].text,"GPRS");
	drawbutton(&mbuttonlist[6]);

	mbuttonlist[7].id = UART_BUTTON;
	mbuttonlist[7].x0 = 60;
	mbuttonlist[7].y0 = 50;
	mbuttonlist[7].wide =80;
	mbuttonlist[7].high = 30;
	mbuttonlist[7].text_x = 25;
	mbuttonlist[7].text_y = 20;
	mbuttonlist[7].textcolor = rgb(255,255,0);
	mbuttonlist[7].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[7].text,"Uart");
	drawbutton(&mbuttonlist[7]);
	
	mbuttonlist[8].id = BLUETOOTH_BUTTON	;
	mbuttonlist[8].x0 = 180;
	mbuttonlist[8].y0 = 10;
	mbuttonlist[8].wide =80;
	mbuttonlist[8].high = 30;
	mbuttonlist[8].text_x = 4;
	mbuttonlist[8].text_y = 20;
	mbuttonlist[8].textcolor = rgb(255,255,0);
	mbuttonlist[8].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[8].text,"BlueTooth");
	drawbutton(&mbuttonlist[8]);

	mbuttonlist[9].id = STEP_MOTOR_BUTTON;
	mbuttonlist[9].x0 = 60;
	mbuttonlist[9].y0 = 10;
	mbuttonlist[9].wide =80;
	mbuttonlist[9].high = 30;
	mbuttonlist[9].text_x = 4;
	mbuttonlist[9].text_y = 20;
	mbuttonlist[9].textcolor = rgb(255,255,0);
	mbuttonlist[9].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[9].text,"StepMotor");
	drawbutton(&mbuttonlist[9]);

	//显示退出窗口
	drawexitbutton();

}

void GPS_win()
{

	
	COLOR mColor;
	mColor = rgb(0,255,0);
	GUI_cls();
	
	
	drawexitbutton();
	buttonnumber = 0;	
	drawtext(70,200,"ＧＰＳ卫星定位窗口",1,3,rgb(255,255,0));
	drawtext(60,150,"类别　方向　数值（度）",1,3,rgb(255,255,0));
	drawtext(60,125,"纬度",1,3,rgb(255,255,0));
	drawtext(60,100,"经度",1,3,rgb(255,255,0));

	
	line(55,157,260,157,mColor);
	line(55,132,260,132,mColor);
	line(55,107,260,107,mColor);
	line(55,82,260,82,mColor);

	
	line(55,157,55,82,mColor);
	line(100,157,100,82,mColor);
	line(170,157,170,82,mColor);
	line(260,157,260,82,mColor);
	

}


void Config_win(void)
{
	int i;
	GUI_cls();

	
	drawexitbutton();
	buttonnumber = 2;	
	drawtext(75,200,"时间和校准设置窗口",1,3,rgb(255,255,0));
	
	mbuttonlist[0].id = TIME_BUTTON;
	mbuttonlist[0].x0 = 60;
	mbuttonlist[0].y0 = 95;
	mbuttonlist[0].wide =80;
	mbuttonlist[0].high = 30;
	mbuttonlist[0].text_x = 25;
	mbuttonlist[0].text_y = 20;
	mbuttonlist[0].textcolor = rgb(255,255,0);
	mbuttonlist[0].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[0].text,"Time");
	drawbutton(&mbuttonlist[0]);
	
	mbuttonlist[1].id = ADJUST_BUTTON;
	mbuttonlist[1].x0 = 180;
	mbuttonlist[1].y0 = 95;
	mbuttonlist[1].wide =80;
	mbuttonlist[1].high = 30;
	mbuttonlist[1].text_x = 16;
	mbuttonlist[1].text_y = 20;
	mbuttonlist[1].textcolor = rgb(255,255,0);
	mbuttonlist[1].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[1].text,"Adjust");
	drawbutton(&mbuttonlist[1]);

}

void Step_Motor_win(void)
{

	char buf[30];
	GUI_cls();
	
	drawexitbutton();
	buttonnumber = 7;	
	drawtext(80,220,"步进电机演示窗口",1,3,rgb(255,255,0));

	drawtext(70,140,"速度：　级",1,0,rgb(255,255,0));	
	drawtext(70,100,"角度：　级",1,0,rgb(255,255,0));	

	mbuttonlist[0].id = MOTOR_SPEED_UP_BUTTON;
	mbuttonlist[0].x0 = 200;
	mbuttonlist[0].y0 = 115;
	mbuttonlist[0].wide =28;
	mbuttonlist[0].high = 30;
	mbuttonlist[0].text_x = 10;
	mbuttonlist[0].text_y = 20;
	mbuttonlist[0].textcolor = rgb(255,255,0);
	mbuttonlist[0].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[0].text,"+");
	drawbutton(&mbuttonlist[0]);
	
	mbuttonlist[1].id = MOTOR_SPEED_DOWN_BUTTON;
	mbuttonlist[1].x0 = 160;
	mbuttonlist[1].y0 = 115;
	mbuttonlist[1].wide =28;
	mbuttonlist[1].high = 30;
	mbuttonlist[1].text_x = 10;
	mbuttonlist[1].text_y = 20;
	mbuttonlist[1].textcolor = rgb(255,255,0);
	mbuttonlist[1].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[1].text,"-");
	drawbutton(&mbuttonlist[1]);

	mbuttonlist[2].id = MOTOR_ANGLE_UP_BUTTON;
	mbuttonlist[2].x0 = 160;
	mbuttonlist[2].y0 = 75;
	mbuttonlist[2].wide =28;
	mbuttonlist[2].high = 30;
	mbuttonlist[2].text_x = 10;
	mbuttonlist[2].text_y = 20;
	mbuttonlist[2].textcolor = rgb(255,255,0);
	mbuttonlist[2].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[2].text,"-");
	drawbutton(&mbuttonlist[2]);
	

	mbuttonlist[3].id = MOTOR_ANGLE_DOWN_BUTTON;
	mbuttonlist[3].x0 = 200;
	mbuttonlist[3].y0 = 75;
	mbuttonlist[3].wide =28;
	mbuttonlist[3].high = 30;
	mbuttonlist[3].text_x = 10;
	mbuttonlist[3].text_y = 20;
	mbuttonlist[3].textcolor = rgb(255,255,0);
	mbuttonlist[3].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[3].text,"+");
	drawbutton(&mbuttonlist[3]);

	mbuttonlist[4].id = START_BUTTON;
	mbuttonlist[4].x0 = 60;
	mbuttonlist[4].y0 = 25;
	mbuttonlist[4].wide =80;
	mbuttonlist[4].high = 30;
	mbuttonlist[4].text_x = 20;
	mbuttonlist[4].text_y = 20;
	mbuttonlist[4].textcolor = rgb(255,255,0);
	mbuttonlist[4].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[4].text,"Start");
	drawbutton(&mbuttonlist[4]);

	mbuttonlist[5].id = STOP_BUTTON;
	mbuttonlist[5].x0 = 180;
	mbuttonlist[5].y0 = 25;
	mbuttonlist[5].wide =80;
	mbuttonlist[5].high = 30;
	mbuttonlist[5].text_x = 24;
	mbuttonlist[5].text_y = 20;
	mbuttonlist[5].textcolor = rgb(255,255,0);
	mbuttonlist[5].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[5].text,"Stop");
	drawbutton(&mbuttonlist[5]);

	drawtext(70,175,"方向：",1,0,rgb(255,255,0));	

	mbuttonlist[6].id = SWITCH_BUTTON;
	mbuttonlist[6].x0 = 134;
	mbuttonlist[6].y0 = 155;
	mbuttonlist[6].wide =95;
	mbuttonlist[6].high = 25;
	mbuttonlist[6].text_x = 24;
	mbuttonlist[6].text_y = 20;
	mbuttonlist[6].textcolor = rgb(255,255,0);
	mbuttonlist[6].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[6].text,"<<<===");
	drawbutton(&mbuttonlist[6]);
	
	//显示速度及角度
	sprintf(buf,"%d",gfreq);
	fillrectangle(120,124,130,140,rgb(0,0,255));	
	drawascii168(120,138,buf,rgb(255,255,0));
	
	sprintf(buf,"%d",5-gAngle);
	fillrectangle(120,84,130,100,rgb(0,0,255));	
	drawascii168(120,98,buf,rgb(255,255,0)); 
	
	//配置与电机有关的接口
	Motor_Entry();
	

}

void uart_win(void)
{
//	if(flushenable)
	GUI_cls();
	
	drawexitbutton();
	buttonnumber = 1;	
	drawtext(120,200,"串口演示窗口",1,3,rgb(255,255,0));
	drawtext(70,150,"请用串口线将主机和目标板相连",1,0,rgb(255,255,0));
	drawtext(40,130,"并用超级终端查看",1,0,rgb(255,255,0));
	drawascii168(170,128,"(115200-8-N-1)",rgb(255,255,0));
	mbuttonlist[0].id = START_BUTTON;
	mbuttonlist[0].x0 = 110;
	mbuttonlist[0].y0 = 50;
	mbuttonlist[0].wide =80;
	mbuttonlist[0].high = 30;
	mbuttonlist[0].text_x = 18;
	mbuttonlist[0].text_y = 20;
	mbuttonlist[0].textcolor = rgb(255,255,0);
	mbuttonlist[0].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[0].text,"Start");
	drawbutton(&mbuttonlist[0]);
	
}	

void usb_win(void)
{
//	if(flushenable)
	GUI_cls();
	
	drawexitbutton();
	buttonnumber = 0;
	drawascii168(110,198,"USB",	rgb(255,255,0));;
	drawtext(138,200,"演示窗口",1,3,rgb(255,255,0));

	drawascii168(70,128,"USB",rgb(255,255,0));
	drawtext(97,130,"已经配置完成，请将",1,0,rgb(255,255,0));
	drawascii168(70,128,",",rgb(255,255,0));
	
	
	drawtext(70,110,"主机和目标板相连并查看！",1,0,rgb(255,255,0));
	
}	

void KBD_win(void)
{
	int i;
	GUI_cls();
	
	drawexitbutton();
	buttonnumber = 0;
	drawtext(65,200,"键盘及数码管演示窗口",1,3,rgb(255,255,0));
	drawtext(70,150,"键盘及数码管已经配置好，",1,0,rgb(255,255,0));
	drawtext(70,130,"请按键盘进行测试！",1,0,rgb(255,255,0));
	
	KBD_Entry();

		


}

void GPRS_win(void)
{
	GUI_cls();
	
	drawexitbutton();
	drawascii168(120,200,"GPRS",rgb(255,255,0));
	buttonnumber = 2;
	phone_index = 0;		//清空phone buffer	
	drawtext(155,200,"演示窗口",1,3,rgb(255,255,0));
	mbuttonlist[0].id = CALL_BUTTON;
	mbuttonlist[0].x0 = 60;
	mbuttonlist[0].y0 = 95;
	mbuttonlist[0].wide =80;
	mbuttonlist[0].high = 30;
	mbuttonlist[0].text_x = 25;
	mbuttonlist[0].text_y = 20;
	mbuttonlist[0].textcolor = rgb(255,255,0);
	mbuttonlist[0].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[0].text,"call");
	drawbutton(&mbuttonlist[0]);
	
	mbuttonlist[1].id = MESSAGE_BUTTON;
	mbuttonlist[1].x0 = 180;
	mbuttonlist[1].y0 = 95;
	mbuttonlist[1].wide =80;
	mbuttonlist[1].high = 30;
	mbuttonlist[1].text_x = 8;
	mbuttonlist[1].text_y = 20;
	mbuttonlist[1].textcolor = rgb(255,255,0);
	mbuttonlist[1].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[1].text,"message");
	drawbutton(&mbuttonlist[1]);
	
	//打开键盘
    keyinit();     
    KB_Enable(TRUE);                      //中断使能函数


}

void CALL_win(void)
{

	int i;
	
	GUI_cls();

	drawexitbutton();
	drawascii168(120,200,"CALL",rgb(255,255,0));

	drawtext(155,200,"演示窗口",1,3,rgb(255,255,0));
	
	drawtext(40,150,"按键盘输入电话号码：",1,0,rgb(255,255,0));
	
	mbuttonlist[0].id = CALL_BUTTON;
	mbuttonlist[0].x0 = 60;
	mbuttonlist[0].y0 = 65;
	mbuttonlist[0].wide =80;
	mbuttonlist[0].high = 30;
	mbuttonlist[0].text_x = 25;
	mbuttonlist[0].text_y = 20;
	mbuttonlist[0].textcolor = rgb(255,255,0);
	mbuttonlist[0].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[0].text,"Call");
	drawbutton(&mbuttonlist[0]);
	
	mbuttonlist[1].id = ESC_BUTTON;
	mbuttonlist[1].x0 = 180;
	mbuttonlist[1].y0 = 65;
	mbuttonlist[1].wide =80;
	mbuttonlist[1].high = 30;
	mbuttonlist[1].text_x = 30;
	mbuttonlist[1].text_y = 20;
	mbuttonlist[1].textcolor = rgb(255,255,0);
	mbuttonlist[1].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[1].text,"Esc");
	drawbutton(&mbuttonlist[1]);

	buttonnumber = 2;
	

}


void Message_win(void)
{
	GUI_cls();
	
	drawexitbutton();
	drawascii168(100,197,"MESSAGE",rgb(255,255,0));
	buttonnumber = 1;	
	drawtext(155,200,"演示窗口",1,3,rgb(255,255,0));
	
	drawtext(40,150,"按键盘输入电话号码：",1,0,rgb(255,255,0));
	drawtext(40,130,"短消息内容为",1,0,rgb(255,255,0));	
	drawascii168(137,127,"HELLO",rgb(255,255,0));
	
	mbuttonlist[0].id = SEND_BUTTON;
	mbuttonlist[0].x0 = 120;
	mbuttonlist[0].y0 = 60;
	mbuttonlist[0].wide =80;
	mbuttonlist[0].high = 30;
	mbuttonlist[0].text_x = 25;
	mbuttonlist[0].text_y = 20;
	mbuttonlist[0].textcolor = rgb(255,255,0);
	mbuttonlist[0].bgcolor = rgb(255,0,255);
	strcopy(mbuttonlist[0].text,"Send");
	drawbutton(&mbuttonlist[0]);
	

}

void drawexitbutton(void)
{
	fillrectangle (300,220,317,237,	rgb(255,0,255));
	line(300,220,317,237,rgb(255,255,0));
	line(300,220,300,237,rgb(255,255,0));
	line(300,220,317,220,rgb(255,255,0));
	line(317,220,317,237,rgb(255,255,0));
	line(300,237,317,220,rgb(255,255,0));
	line(300,237,317,237,rgb(255,255,0));
}

void Config_time_win(void)
{

	char timestr[20];
	GUI_cls();
	drawexitbutton();
	KB_Enable(TRUE);
	buttonnumber = 0;
	gTimeIndex = 0;	
	drawtext(100,200,"时间设置窗口",1,3,rgb(255,255,0));
	drawtext(70,140,"时间：",1,0,rgb(255,255,0));
	sprintf(timestr,"%d%d:%d%d",0,0,0,0);
	drawascii168(120,138,timestr,rgb(255,255,0));	
	line(120,122,128,122,rgb(255,0,0));
	
	strcpy(timestr,"星期日");
	drawtext(180,138,timestr,1,0,rgb(255,255,0));
	drawtext(70,70,"按数字键盘输入数据！",1,3,rgb(255,255,0));
}
	
void changewindows(int x,int y)
{
	int i;
	switch(win_ID)
	{
	case MAIN_WIN:				//屏保
		win_ID = MAIN_MENU;
		flushenable = 1; 
		mainmenu();;   //需要刷新窗口
		break;
	case MAIN_MENU:
		 switch(findbuttonid(x,y))
		 {
		 case GPRS_BUTTON:
			win_ID = GPRS_WIN;
//			flushenable = 1;   //需要刷新窗口
			GPRS_win();
			break;
		case BLUETOOTH_BUTTON:
//			win_ID = BLUETOOTH_WIN;
//			flushenable = 1;   //需要刷新窗口
			break;
		case UART_BUTTON:
			win_ID = UART_WIN;
			uart_win();
			for(i=0;i<20000;i++);
			flushenable = 0;
			break;
		case USB_BUTTON:
			win_ID = USB_WIN;
			usb_win();
			for(i=0;i<20000;i++);
			flushenable = 0;
			break;
			
		case KBD_BUTTON:
			win_ID = KBD_WIN;
			KBD_win();
			for(i=0;i<20000;i++);
			flushenable = 0;
			break;
			
		case IIS_BUTTON:

			playwav();
			break;
		case CONFIG_BUTTON:
			win_ID = CONFIG_WIN;
			Config_win();
			for(i=0;i<20000;i++);
			flushenable = 0;
			break;
			
		case STEP_MOTOR_BUTTON:
			win_ID = STEPMOTOR_WIN;
			Step_Motor_win();
			for(i=0;i<20000;i++);
			flushenable = 0;
			break;
		case GPS_BUTTON:
			win_ID = GPS_WIN;
			GPS_win();
			for(i=0;i<20000;i++);
			flushenable = 0;
			break;
			
		case EXIT_BUTTON:
			win_ID = MAIN_WIN;
			flushenable = 0;
			buttonnumber =0;            //在封面上没有按钮
			LCD_Entry();
			break;

		}
		break;
	case GPRS_WIN:
		if(findbuttonid(x,y) == EXIT_BUTTON) 
		{
		win_ID = MAIN_MENU;
		mainmenu();
		}
		if(findbuttonid(x,y) == CALL_BUTTON) 
		{
		win_ID = CALL_WIN;
		clearbuttonlist();
//		for(i=0;i<1000000;i++);	
		CALL_win();
		}
		if(findbuttonid(x,y) == MESSAGE_BUTTON) 
		{
		win_ID = MESSAGE_WIN;	
		Message_win();
		}
		break;
	case UART_WIN:
		if(findbuttonid(x,y) == EXIT_BUTTON)
		{
			win_ID = MAIN_MENU;
			mainmenu();
		}
		if(findbuttonid(x,y) == START_BUTTON)
		{

                        CONSOL_Select(0);
                        CONSOL_Init(115200);
		   	CONSOL_Printf("\n\r***************************************************************");
			CONSOL_Printf("\n\r*        Copyright Shenzhen Watertek S.&T. Co.,Ltd  2005      *");  
			CONSOL_Printf("\n\r*                    All Rights Reserved.        	      *");
		   	CONSOL_Printf("\n\r***************************************************************");
	
		}
		break;
	case USB_WIN:
		if(findbuttonid(x,y) == EXIT_BUTTON)
		{
			win_ID = MAIN_MENU;
			mainmenu();
		}
		break;
	case KBD_WIN:
		if(findbuttonid(x,y) == EXIT_BUTTON)
		{
			win_ID = MAIN_MENU;
			mainmenu();
		}
		break;
	case CALL_WIN:
		if(findbuttonid(x,y) == EXIT_BUTTON)
		{
			win_ID = GPRS_WIN;
			phone_index =0;    //清空电话号码buffer
			GPRS_win();
		}
		
		if(findbuttonid(x,y) == CALL_BUTTON) 
		{

		call_demo(phone_num);
		}
		if(findbuttonid(x,y) == ESC_BUTTON) 
		{

		disconnect_phone();
		}
		break;
		
	case MESSAGE_WIN:
		if(findbuttonid(x,y) == EXIT_BUTTON)
		{
			win_ID = GPRS_WIN;
			phone_index =0;    //清空电话号码buffer
			GPRS_win();
		}
		
		if(findbuttonid(x,y) == SEND_BUTTON) 
		{
//		win_ID = CALL_WIN;
		sendmessage_demo(phone_num);
		}
//		if(findbuttonid(x,y) == ESC_BUTTON) 
//		{
//		win_ID = CALL_WIN;
//		disconnect_phone();
//		}
		break;
		
	case CONFIG_WIN:
		switch(findbuttonid(x,y))
		{
			case EXIT_BUTTON:
			{		
				win_ID = MAIN_MENU;
				
				mainmenu();
				phone_index =0;    //清空电话号码buffer
				break;
			}
			
			case ADJUST_BUTTON: 
			{
				int k;
				for(k=0;k<25;k++)
				{
					IIC_write(k,0);
				} 
				drawtext(110,50,"请重新启动！",1,0,rgb(255,255,0));			
				break;
			}
			case TIME_BUTTON: 
			{
				win_ID = TIME_CONFIG_WIN;
				Config_time_win();
				break;
			}
		}		
		break;
	

	case STEPMOTOR_WIN:
		motorctrl(findbuttonid(x,y));
		break;
	case TIME_CONFIG_WIN:
		switch(findbuttonid(x,y))
		{
			case EXIT_BUTTON:
			{		
			win_ID = CONFIG_WIN;
			time.hour = (gTimestr[0] - '0')*10 + (gTimestr[1] - '0');
			time.min  = (gTimestr[2] - '0')*10 + (gTimestr[3] - '0');
			time.weekday = gTimestr[4] - '0';
			if((time.weekday))writeRTC(&time);	
			Config_win();
//			phone_index =0;    //清空电话号码buffer
			break;
			}
		
		}
		break;
	case GPS_WIN:
		switch(findbuttonid(x,y))
		{
		case EXIT_BUTTON:
		{		
			win_ID = MAIN_MENU;
			mainmenu();
			break;

		}
               }
	      break;


	}
				
	
}

int findbuttonid(int x,int y)
{
	int i;
	for(i=0;i<buttonnumber;i++)
	{
		if(x>=mbuttonlist[i].x0&&x<=(mbuttonlist[i].x0+mbuttonlist[i].wide)&&y>=mbuttonlist[i].y0&&y<=(mbuttonlist[i].y0+mbuttonlist[i].high))
			{
				fillreverserectangle (mbuttonlist[i].x0,mbuttonlist[i].y0,mbuttonlist[i].x0+mbuttonlist[i].wide,mbuttonlist[i].y0+mbuttonlist[i].high);
				waitforstylusup();		
				drawbutton(&mbuttonlist[i]);
				return mbuttonlist[i].id ;
			}
	}
	if(x>=290&&y>210)  return EXIT_BUTTON;
	return 0;
}

void motorctrl(int id)
{
    switch(id)
    {
	case EXIT_BUTTON:
	{		
		win_ID = MAIN_MENU;
		MotorOff(TIMER2);	//关闭电机
		mainmenu();
		phone_index =0;    //清空电话号码buffer
		break;
	}
	case MOTOR_SPEED_UP_BUTTON: 
	{
		char buf[30];
		gfreq++;
		if(gfreq >=4) gfreq =4;
		SetClkFreq(gfreq*50,TIMER2);
		sprintf(buf,"%d",gfreq);
		fillrectangle(120,124,130,140,rgb(0,0,255));	
		
		drawascii168(120,138,buf,rgb(255,255,0));
		break;
	}
	case MOTOR_SPEED_DOWN_BUTTON: 
	{
		char buf[30];
		gfreq--;
		if(gfreq <=1) gfreq =1;
		SetClkFreq(gfreq*50,TIMER2);
	
		sprintf(buf,"%d",gfreq);
		fillrectangle(120,124,130,140,rgb(0,0,255));	
		
		drawascii168(120,138,buf,rgb(255,255,0));
		break;	
	}
	case MOTOR_ANGLE_UP_BUTTON: 
	{
		char buf[30];
		gAngle++;
		if(gAngle >=4) gAngle =4;
		AngleCtrl(gAngle);
		sprintf(buf,"%d",5-gAngle);
		fillrectangle(120,84,130,100,rgb(0,0,255));	
		drawascii168(120,98,buf,rgb(255,255,0)); 
		break;	
	}
	case MOTOR_ANGLE_DOWN_BUTTON: 
	{
		char buf[30];
		gAngle--;
		if(gAngle <=1) gAngle =1;
		AngleCtrl(gAngle);
		sprintf(buf,"%d",5-gAngle);
		fillrectangle(120,84,130,100,rgb(0,0,255));	
		drawascii168(120,98,buf,rgb(255,255,0)); 
		break;	
	}
	case START_BUTTON: 
	{
		MotorOn(TIMER2);
		DirectoryCtrl(gDirectoryCtrl);		//改变方向
//		drawascii168(120,40,"START_BUTTON",rgb(255,255,0));
		break;	
	}
	case STOP_BUTTON: 
	{
		MotorOff(TIMER2);
		DirectoryCtrl(gDirectoryCtrl);		//改变方向		
//		drawascii168(120,40,"STOP_BUTTON",rgb(255,255,0));
		break;	
	}
	case SWITCH_BUTTON: 
	{
		if(gDirectoryCtrl == 0)
		{
			gDirectoryCtrl =1;
			strcopy(mbuttonlist[6].text,"===>>>");		//修改显示的按钮界面
		}			
		else 
		{
			gDirectoryCtrl =0;
			strcopy(mbuttonlist[6].text,"<<<===");		//修改显示的按钮界面
		}

		DirectoryCtrl(gDirectoryCtrl);

		drawbutton(&mbuttonlist[6]);

//		drawascii168(120,40,"STOP_BUTTON",rgb(255,255,0));
		break;	
	}

  }
}
