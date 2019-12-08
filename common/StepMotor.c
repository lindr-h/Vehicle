#include <string.h>
#include "2410addr.h"
#include "StepMotor.h"

int gAngle = 1;
int gfreq = 50;

//#define SPEEDCTRL 50

 void __irq TIMER2_LISR(void)
{	
	int temp;
	
	AngleCtrl(gAngle);
	SetClkFreq(gfreq);
	
	temp = rSUBSRCPND;
    rSUBSRCPND=temp;
	temp = rSRCPND;
	rSRCPND = temp;
	temp = rINTPND;
	rINTPND = temp;
}

void SetTimer(void)
{
	rGPBUP |= 0x4;
	rGPBCON = rGPBCON & 0xffff0f |(1<<5);//select timer2(GPB2)
}

void EnableController(void)
{	
	rGPFUP = 0x1;
	rGPFCON = rGPFCON & 0xfffd |1;//GPF0 OUTPUT
	rGPFDAT |= 0x1;                                 //enable TA8435H
}

//启动
void MotorOn(void)
{	
	EnableController();
		
	SetTimer();
	AngleCtrl(gAngle);		      	
	rTCFG0 = 0xff00;//set timer2 Prescaler=255		  	
	rTCFG1 &= 0xffff0fff;//set timer2 mux  divider value=2		  	
	SetClkFreq(gfreq);
	rTCON |= (1<<15)|(1<<13);//auto load,manual update		  	
	rSRCPND = 0x1000;//Timer2 interrupt request		  	
	rINTMSK &= ~(BIT_TIMER2);//enable Timer2 interrupt
	rTCON = 0x9000;//auto reload;start for timer2;clear manual update bit;close inverter
}

//关闭 
void MotorOff(void)
{
	rTCON = 0x0;//stop timer
	rINTMSK |= BIT_TIMER2;//disable interrupt
}


//角度
void AngleCtrl(int flag)
{	
	rGPGUP |= 0x808;
	rGPGCON = rGPGCON & 0xff7fff7f |(1<<6)|(1<<22);
	switch(flag)
	{
		case 1:
			rGPGDAT &= 0xf7f7;
			break;
		case 2:
			rGPGDAT = rGPGDAT & 0xf7ff |(1<<3);
			break;
		case 3:
			rGPGDAT = rGPGDAT & 0xfff7 |(1<<11);
			break;
		case 4:
			rGPGDAT |=0x808;
			break;
		default:
			break;
	}
}


//频率
void SetClkFreq(int freq)
{
	int tclk=0,count=0;
    
	tclk = MOTORPCLK/(PRESCALE+1)/DIVIDER;
    count = tclk/freq;

	rTCNTB2 = count;
    rTCMPB2 = count/LEVEL;	
}

//方向
void DirectoryCtrl(int flag)
 {
 	rGPFCON = rGPFCON & 0xffdf | (1<<4);
 	
	if(flag)
		rGPFDAT |= 0x4;		
	else
		rGPFDAT &= 0xfb;
		
 }


