#include "def.h"

#define GUI_VERTICAL	0
#define GUI_HORIZONTAL	11

void LCD_Init(void);
void WIN_WaterTek(char *ch);

int RGB(unsigned char Red,unsigned char Green,unsigned char Blue);
unsigned short ColorPC(unsigned char Msb, unsigned char Lsb);
int setpixel(int x,int y,unsigned short pencolor);
COLOR getpixel(int x,int y);

int ShowImage(unsigned char *p , int StartX, int StratY, int mode);

void line(int x0,int y0,int x1,int y1,COLOR mcolor);
void clrsrc(COLOR mcolor);
void fillrectangle(int x0,int y0,int x1,int y1,COLOR mcolor);
void DrawFrame(int xS, int yS, int xE, int yE, int width, COLOR mcolor);
void fillreverserectangle(int x0,int y0,int x1,int y1);

int drawtext(int x0,int y0,char *c,int zoom,int space,int pencolor);
int drawascii168(int x0,int y0,char *c,int zoom,int space,int pencolor); 
int drawletter(int x0,int y0,char *c,int zoom,int space,int pencolor);
int Rewriteletter(int x0,int y0,char *c,int zoom,int space,int pencolor,int colorin);
void PrintTextEdit(char *c,int pencolor,int x_start,int y_start,int x_end,int y_end,int EditID);
int LcdOrientation(void);
//void LcdOrientationInit(void);
void LcdHorizontal(void);
void LcdVertical(void);

void _PutCstn12Bit(int x,int y,unsigned short c);
unsigned short _GetCstn12Bit(int x,int y);