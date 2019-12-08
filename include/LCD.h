#ifndef __LCD_H__
#define __LCD_H__


#include "def.h"
#include "2410lib.h"

#define MAX	11

unsigned short rgb(int red,int green,int blue);
void LCD_Entry(void);
void fillrectangle(int x0,int y0,int x1,int y1,COLOR mcolor);
int LCD_Init(void);
void INT_ADC_Enable(int flag);
void clrsrc(COLOR mcolor);
int setpixel(int x,int y,unsigned short pencolor);
COLOR getpixel(int x,int y);
unsigned short rgb(int red,int green,int blue);
 int drawtext(int x0,int y0,char *c,int zoom,int space,int pencolor) ;
int drawascii168(int x0,int y0,char *c,int pencolor);
void line(int x0,int y0,int x1,int y1,COLOR mcolor); 
void showimage(unsigned char *p);
void ascii_168(int x0,int y0,char *code,int pencolor);
#endif
