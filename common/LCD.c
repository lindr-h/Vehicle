/*************************************************************************/
/*                                                                       */
/*               Copyright Shenzhen Watertek S.&T. Co.,Ltd  2002         */
/*                         All Rights Reserved.                          */
/*                                                                       */
/* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS  */
/* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS   */
/* SUBJECT TO LICENSE TERMS.                                             */
/*                                                                       */
/*************************************************************************/

/*************************************************************************/
/*                                                                       */
/* FILE NAME                                               VERSION       */
/*                                                                       */
/*      usbmain.c                                   S3c2410 USB device 1.0*/
/*                                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains macro and main routines 		 */
/*                                                                       */
/* AUTHOR                                                                */
/*    	Zhi-gang yang   shenzhen watertek                                */
/*                                                                       */
/*                                                                       */
/* DATA STRUCTURES                                                       */
/*                                                                       */
/*      Ep3Handler                   EP3 interrupt handler          	 */
/*      PrintEpoPkt                  print ep out packet for debug       */
/*      RdPktEp3_CheckSum            No	use 				 */
/*      IsrDma2              	     DMA interrupt handler,No use in this*/
/*					 case  				 */
/*      ClearEp3OutPktReady          clear endpoint 3 packet out tag     */
/*                                                                       */
/* FUNCTIONS                                                             */
/*                                                                       */
/*      None                                                             */
/*                                                                       */
/* DEPENDENCIES                                                          */
/*                                                                       */
/*      None	                          				 */
/*                                                                       */
/* HISTORY                                                               */
/*                                                                       */
/*         DATE                    REMARKS                               */
/*                                                                       */
/*      2004-12-27      Created initial version 1.0                      */
/*************************************************************************/
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "2410addr.h"
#include "def.h"
#include "lcd.h"
#include "2410lib.h"
#include "RTC.h"

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      C_Entry			                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      C entry function				                 */
/*                                                                       */
/* AUTHOR                                                                */
/*                                                                       */
/*      Zhi-gang Yang          shenzhen watertek                         */
/*                                                                       */
/* INPUTS                                                                */
/*      None                                                             */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None      	                                                 */
/*                                                                       */
/* HISTORY                                                               */
/*                                                                       */
/*         DATE                    REMARKS                               */
/*                                                                       */
/*      2004-12-28      Created initial version 1.0                      */
/*************************************************************************/
void LCD_Entry(void)
{
//	int i;
	TIME time;
	int flag =0;
	char weekdaystr[20];
	char timestr[60];
	unsigned char *ptr;
	Led4_On_or_Off(TRUE);
	ptr =(unsigned char*) IMAGEBUFFER;
	showimage(ptr);	
	//add RTC
	readRTC (&time);
	
	if(time.weekday == 1)  strcpy(weekdaystr,"星期一");
	else if(time.weekday == 2)  strcpy(weekdaystr,"星期二");
	else if(time.weekday == 3)  strcpy(weekdaystr,"星期三");
	else if(time.weekday == 4)   strcpy(weekdaystr,"星期四");
	else if(time.weekday == 5)   strcpy(weekdaystr,"星期五");
	else if(time.weekday == 6)   strcpy(weekdaystr,"星期六");
	else if(time.weekday == 7)   strcpy(weekdaystr,"星期日");
	else 
		{
			strcpy(weekdaystr,"时间没有设置正确");
			flag =1 ;
		}
	
	if(flag) 		drawtext(185,22,weekdaystr,1,0,rgb(255,0,0));
	else	drawtext(265,22,weekdaystr,1,0,rgb(255,0,0));
	
	sprintf(timestr,"%d%d:%d%d",time.hour/10,time.hour%10,time.min/10,time.min%10);
	
	drawascii168(270,40,timestr,rgb(255,0,0));
	
	
}

void INT_ADC_Enable(int flag)
{
	int temp;
	if(flag == FALSE)
	{
		temp = rINTSUBMSK;
		temp |=(1<<10);
		rINTSUBMSK = temp;
	
		temp = rINTMSK;
		temp |=0x80000000;
		rINTMSK = temp;
	}
	else 
	{
		temp = rINTSUBMSK;
		temp = 0x5ff;
		rINTSUBMSK = temp;
	
		temp = rINTMSK;
		temp &= ~(0x80000000) ;
		rINTMSK = temp;
	}
}


int LCD_Init(void)
{
   rLCDCON2=0x14FC081;
   rLCDCON3=0x188EF0F;
   rLCDCON4=0xD;
   rLCDCON5=0xB09;
   rLCDSADDR1=0x18000000;
   rLCDSADDR2=0x12c00;
   rLCDCON1=0x479;
   rLCDSADDR3=0x0;
   rREDLUT=0x0;
   rGREENLUT=0x0;
   rBLUELUT=0x0;
   rDITHMODE=0x0;
   rTPAL=0x0;
   rLCDINTPND=0x0;
   rLCDSRCPND=0x0;
   rLCDINTMSK=0x3;
   rLPCSEL=0x4;
   rADCTSC=0xd3;
   rADCDLY=0xc350;
   INT_ADC_Enable(TRUE);
   
   return 0;
}



unsigned short rgb(int red,int green,int blue)
{
	unsigned short pencolor;
	/*compose the color depend the red, blue and green*/
	pencolor=(((red&(~(~0<<RED_BITNUM)))<<RED_SHIFT)|((green&(~(~0<<GREEN_BITNUM)))<<GREEN_SHIFT)\
	                                                   |((blue&(~(~0<<BLUE_BITNUM)))<<BLUE_SHIFT));
	return pencolor;
}
		
int setpixel(int x,int y,unsigned short pencolor)
{
	int movetop;   /*THE PIXEL ADDRESS*/
	short *p;
	if (x<0||x>(int)x_limit) return ERROR;
	if (y<0||y>(int)y_limit) return ERROR;
	/*指向所要显示的像素偏移地址*/
	movetop=(x*x_size+y)*2;//一个像素占两个字节   
	p=(short *)(VideoAddrStart+movetop);
	*p=pencolor;
	return 1;
}

COLOR getpixel(int x,int y)
{
	int movetop;   /*THE PIXEL ADDRESS*/
	short *p;
	if (x<0||x>(int)x_limit) return ERROR;
	if (y<0||y>(int)y_limit) return ERROR;
	/*指向所要显示的像素偏移地址*/
	movetop=(x*x_size+y)*2;//一个像素占两个字节   
	p=(short *)(VideoAddrStart+movetop);
	return *p;
}


void clrsrc(COLOR mcolor)
{
short *p;
for(p=(short*)VideoAddrStart;p<(short*)VideoAddrEnd;p++) *p=mcolor;
return ;
}

void ascii_168(int x0,int y0,char *code,int pencolor)
{
	 /*define the code group for compare the code*/
	 char mask[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	  char mat[16];
	  char *hzpos;
	  int i,k,m,offset;
	  offset=(*code)*16;
	  hzpos=(char *)(asciibufbase+offset);
	  /*transmit the lattice message to the group*/
	  for (m=0;m<16;m++)
  	  {
  	  	  mat[m]=*hzpos;
  		  hzpos++;
  	   }
  	  /*display the character depend on the messsage of the group*/
  	  for (i=0;i<16;i++)
	  		for (k=0;k<8;k++)
	  		{
	  		 	if (mask[k%8]&mat[i])
	  		 	{
	 			 setpixel(x0+k,y0-i,pencolor);

	 			 }
 	   		}
}

int drawascii168(int x0,int y0,char *c,int pencolor)             
{    
	int x,y,counter;
	x=x0;
	y=y0;
	counter=0;
	while(*c!='\0')    /*while don't reach to the end of the string*/
	{
		while(x<320 && (*c!='\0'))   /*while the X coordinate don't exceed the scope of X limit*/
		{
			ascii_168(x,y,c,pencolor);  /*display the character*/
			x+=8;
			c+=1;
			counter+=1;
		}
        	x=0;  /*exchange the next row*/                                              
        	y+=16;
    }
     return(counter);
}       

void ascii_1616(int x0,int y0,char *code,int pencolor)
{
	/*define the code group for compare the code*/
	 char mask[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	  char mat[32];
	  char *hzpos;
	  int i,j,k,m,offset;
	  /*computer the excurison of the base address*/
	  offset=(*code)*32;
	  hzpos=(char *)(asciibufbase+offset);
	  /*get the character lattice message*/
	  for (m=0;m<32;m++)
  	  {
  	  	mat[m]=*hzpos;
  		  hzpos++;
  	   }
  	   /*display the character*/
  	  for (i=0;i<16;i++)
	  	for (j=0;j<2;j++)
	  		for (k=0;k<8;k++)
	  		{
	  		 	if (mask[k%8]&mat[2*i+j])
	 			 setpixel(x0+8*j+k,y0+i,pencolor);
 	   		}
}


int drawascii1616(int x0,int y0,char *c,int pencolor)             
{    
	int x,y,counter;
	x=x0;
	y=y0;
	counter=0;
	/*while don't reach to the end of the string*/
	while(*c!='\0')
	{
		while(x<320 && (*c!='\0'))  /*while the X coordinate don't exceed the scope of X limit*/ 
		{
			ascii_1616(x,y,c,pencolor);  /*display the character*/
			x+=16;
			c+=1;
			counter+=1;
		}
        	x=0;    /*exchange the next row*/                                              
        	y+=16;
      	}
     return(counter);
}       

void get_hz(int x0,int y0,char incode[],int zoom,int pencolor)                   
{	 
	  /*define the code group for compare the code*/
	  char mask[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	  char mat[32];
	  char wh,qh;   /*the position code of the chinese characters*/
	  char *hzpos;
	  int i,j,k,m,offset;
	  /*get the position code from the ISN*/
	  qh=incode[0]-0xa0;
	  wh=incode[1]-0xa0;
	  /*get the excuriosn of chinese character and add it to the base address of chinese characters storeroom*/
	  offset=(94*(qh-1)+(wh-1))*32;
	  hzpos=(char *)(hzbufbase+offset);
	  /*get the chinese character lattice message*/
	  for (m=0;m<32;m++)
  	  {
  	  	  mat[m]=(*hzpos);
  		  hzpos++;
  	   }
  	   /*display the chinese character*/
  	  for (i=0;i<16;i++)
	  	for (j=0;j<2;j++)
	  		for (k=0;k<8;k++)
	  		{
	  		 	if (mask[k%8]&mat[2*i+j])
	  		 	{
	 			 setpixel(x0+zoom*(8*j+k),y0-zoom*i,pencolor);
	 			 }
 	   		}
}
 
 
 int drawtext(int x0,int y0,char *c,int zoom,int space,int pencolor)             
{    
	int x,y,counter;
	x=x0;
	y=y0;
	counter=0;
	while(*c!='\0')
	{
		/*if don't reach to the end of the string display the character in LCD*/
		while(x<320 && (*c!='\0'))
		{
			get_hz(x,y,c,zoom,pencolor);  /*display the Chines character*/
			x+=16*zoom+space;
			c+=2;
			counter+=1;
		}
        	x=0;                                                
        	y+=32;
      	}
     return(counter);
}       
 

void line(int x0,int y0,int x1,int y1,COLOR mcolor)
{
	int x;
	int y;
	if(x0==x1)
	{	if(y0>y1)
		{
		y=y0;y0=y1;y1=y;
	       }
	       for(y=y0;y<=y1;y++)
	       	setpixel(x0,y,mcolor);
	       return;
	 }
	 
	//swap
	if(x0>x1)
	{
		x=x0;x0=x1;x1=x;
		y=y0;y0=y1;y1=y;
	}
	for(x=x0;x<=x1;x++)
	{
		y=(int)(((float)(x-x0))*(y1-y0)/(x1-x0) + y0);
		setpixel(x,y,mcolor);
	}
	
}


void fillrectangle(int x0,int y0,int x1,int y1,COLOR mcolor)
{
	int x;
	int y;
	for(x=x0;x<=x1;x++)
	   for(y=y0;y<=y1;y++) setpixel(x,y,mcolor);
}

void showimage(unsigned char *p)
{
   
   int i,j;
   unsigned char red;
   unsigned char green;
   unsigned char blue;
   unsigned short color;
   p += 54;
   for(j=0;j<240;j++)
   for(i=0;i<320;i++)
  {

	blue = p[0];
	green = p[1];
	red = p[2];
	p +=3;
	
	color = rgb (red ,green ,blue);
    setpixel(i,j,color);
    }
    
}   
