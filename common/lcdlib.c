#include <string.h>
#include "lcdlib.h"
#include "2410addr.h"
#include "2410lib.h"
#include "def.h"
//#include "win_gui.h"

int TextEdit_x = 5,TextEdit_y = 32,TextEdit_id = 0;
int g_xMax = 240;
int g_yMax = 320;
int mGuiOrientation = GUI_VERTICAL;

/*===================STN 7寸微彩屏==========================*/

#ifdef STN
int (*frameBuffer12Bit)[640*6/8];

//STN微彩LCD初始化函数 (640*480)
void LCD_Init(void)
{
 	rLCDCON1=0x548;
    	// 8-bit single scan,8bpp CSTN,ENVID=off
	rLCDCON2=0x790000;
	rLCDCON3=0xef10;
	rLCDCON4=0xd00;
	rLCDCON5=0;
	
	rLCDSADDR1 = 0x18000000;          //显存地址
	rLCDSADDR2 = 0x71ac0;
	rLCDSADDR3 = 0xf01e0;
	rDITHMODE  = 0x12210;
	rREDLUT  =0;
	rGREENLUT=0;
	rBLUELUT =0;
	rLCDCON1|=1; // ENVID=ON
	rGPGUP = 0xffff;
	rGPGDAT = 0xa8ff;
	rGPGCON = 0xff95fdba;
	
}
//清屏函数
void clrsrc(COLOR mcolor)
{
	unsigned int *p;
	unsigned int P1,P2,P3;
	int R,G,B;
	R = (mcolor & 0xf00) >> 8;
	G = (mcolor & 0x0f0) >> 4;
	B = (mcolor & 0x00f);
	
	P1 = (unsigned int) ((R<<28)|(G<<24)|(B<<20)|(R<<16)|(G<<12)|(B<<8)|(R<<4)|G);
	P2 = (unsigned int) ((B<<28)|(R<<24)|(G<<20)|(B<<16)|(R<<12)|(G<<8)|(B<<4)|R);
	P3 = (unsigned int) ((G<<28)|(B<<24)|(R<<20)|(G<<16)|(B<<12)|(R<<8)|(G<<4)|B);


	
	for(p=(unsigned int*)VideoAddrStart;p<(unsigned int*)VideoAddrEnd;)
		{
	 		*p = P1;
	 		p++;
	 		*p = P2;
	 		p++;
	 		*p = P3;
	 		p++;
	 	}
	return ;
}
//画点函数
int setpixel(double x,double y,unsigned short pencolor)
{
	_PutCstn12Bit(x,Y_MAX-y,pencolor);
	
	return 1;
	
}

void _PutCstn12Bit(int x,int y,unsigned short c)
{
    U32 z;
    frameBuffer12Bit= (int (*)[640*6/8])VideoAddrStart;
    z=((x)%8);
    if(x<640&& y<485)
    {

        if((z%3)!=2)
            frameBuffer12Bit[(y)][(x)*3/8]=( frameBuffer12Bit[(y)][(x)*3/8] & ~(0xfff00000>>(((z/3)*4)+((z)%3)*12) )|((c&0xfff)<<(20-(((z/3)*4)+((z)%3)*12))) );
        else
        {
            if(z==2)
            {
                frameBuffer12Bit[(y)][(x)*3/8]=( (frameBuffer12Bit[(y)][(x)*3/8]
		    & ~(0xff)) | ((c&0xff0)>>4)  ); 
                frameBuffer12Bit[(y)][((x)*3/8)+1]=( (frameBuffer12Bit[(y)][((x)*3/8)+1]
		    & ~(0xf0000000)) | ((c&0xf)<<28)  );
            }   
            else if(z==5)
            {
                frameBuffer12Bit[(y)][(x)*3/8]=( (frameBuffer12Bit[(y)][(x)*3/8]
	            & ~(0xf)) | ((c&0xf00)>>8)  );
                frameBuffer12Bit[(y)][((x)*3/8)+1]=( (frameBuffer12Bit[(y)][((x)*3/8)+1]
	            & ~(0xff000000)) | ((c&0xff)<<24)  );
            }   
        }
    }
}

//取色函数
COLOR getpixel(int x,int y)
{
	int color;
	color = _GetCstn12Bit(x,y);
	return color;
}

unsigned short _GetCstn12Bit(int x,int y)
{
    int z;
    int total;
    int temp_h;
    int temp_l;
    frameBuffer12Bit= (int (*)[640*6/8])VideoAddrStart;
    
    z=((x)%8);
    if(x<640&& y<485)
    {

        if((z%3)!=2)
        {
            temp_h =( frameBuffer12Bit[(y)][(x)*3/8] & (0xfff00000>>(((z/3)*4)+((z)%3)*12) ) );
            total = temp_h>>(20-((z/3)*4+(z%3)*12));
            }
        else
        {
            if(z==2)
            {
                temp_h=( (frameBuffer12Bit[(y)][(x)*3/8]& (0xff))  ); 
                temp_l=( (frameBuffer12Bit[(y)][((x)*3/8)+1]& (0xf0000000)));
                total = temp_h<<4+temp_l>>28;
            }   
            else if(z==5)
            {
                temp_h =( (frameBuffer12Bit[(y)][(x)*3/8]
	            & (0xf))  );
                temp_l =( (frameBuffer12Bit[(y)][((x)*3/8)+1]
	            & (0xff000000))  );
	            total = temp_h<<8 + temp_l>>24;
            }   
        }
        return (unsigned short)total;
    }
    return 0;
}
//24bit红绿蓝转换为12bit
int RGB(unsigned char Red,unsigned char Green,unsigned char Blue)
{
	return ( ((Red&0xf0) << 4) | (Green&0xf0) | ((Blue&0xf0) >> 4) );
}

unsigned short ColorPC(unsigned char Msb, unsigned char Lsb)
{
	unsigned short color;
	unsigned short red, green, blue;
	
	color = (Msb << 8) | Lsb;
	red = (color & 0x3C00) >> 2;
	green = (color & 0x1E0) >> 1;
	blue = (color & 0xF);
	
	color = red | green | blue;
	
	return color;
}

/*===================TFT 3.4寸真彩屏==========================*/
#else
//TFT真彩LCD初始化函数 (320*240)
void LCD_Init(void)
{
	rLCDCON2=0x14FC081;
//	rLCDCON3=0x188EF0F;				// 旧屏
	rLCDCON3=0x6<<19 | 0xEF0F;		// 新屏
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
 
}

//画点函数
int setpixel(int x,int y,unsigned short pencolor)
{
	int movetop;   // THE PIXEL ADDRESS
	short *p;
	
	if (x<0||x>=g_xMax) return ERROR;
	if (y<0||y>=g_yMax) return ERROR;

	if (mGuiOrientation == GUI_HORIZONTAL)
		movetop=(x*g_yMax+y)<<1;					// 指向所要显示的像素偏移地址(一个像素占两个字节)
	else
		movetop=((g_yMax-y)*g_xMax-(g_xMax-x))<<1;	// 指向所要显示的像素偏移地址(一个像素占两个字节)
		
	p=(short *)(VideoAddrStart|movetop);
	*p=pencolor;
	return 1;
}

// 取色函数
COLOR getpixel(int x,int y)
{
	int movetop;   // THE PIXEL ADDRESS

	short *p;

	if (x<0||x>=g_xMax) return ERROR;
	if (y<0||y>=g_yMax) return ERROR;
	
	if (mGuiOrientation == GUI_HORIZONTAL)
		movetop=(x*g_yMax+y)<<1;					// 指向所要显示的像素偏移地址(一个像素占两个字节)
	else
		movetop=((g_yMax-y)*g_xMax-(g_xMax-x))<<1;	// 指向所要显示的像素偏移地址(一个像素占两个字节)
	
	p=(short *)(VideoAddrStart|movetop);
	return *p;
}

// 清屏函数
void clrsrc(COLOR mcolor)
{
	short *p;
	for(p=(short*)VideoAddrStart;p<(short*)VideoAddrEnd;p++) *p = mcolor;
}

// 24bit红绿蓝转换为16bit
int RGB(unsigned char Red,unsigned char Green,unsigned char Blue)
{
	return ( ((Red>>3)<<11) | ((Green>>2)<<5) | (Blue >> 3) );
}

unsigned short ColorPC(unsigned char Msb, unsigned char Lsb)
{
	unsigned short color;
	unsigned short temp;
	
	temp = (Msb << 8) | Lsb;
	color = (temp << 1) & 0xFFC0;
	color |= temp & 0x1F;
	
	return color;
}

#endif

/*===================共用函数==========================*/


// 画图函数,图像可以是16位,或24位,但尺寸必需为320*240
int ShowImage(unsigned char *p , int StartX, int StratY, int mode)
{
	int x,y;
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned short color;

	unsigned short fileType;
	unsigned short imageColorBit;
	int height;
	int width;
//	int imageSize;
	int patch = 0;

	imageColorBit = p[28];								// 取得图片的颜色位数
	fileType = LINK_HWORD(p[1], p[0]);					// 文件类型
//	imageSize = LINK_WORD(p[37], p[36], p[35], p[34]);	// 取得图象的总大小
	height = LINK_WORD(p[25], p[24], p[23], p[22]);		// 取得图象的宽度，单位是象素
	width  = LINK_WORD(p[21], p[20], p[19], p[18]);		// 取得图象的宽度，单位是象素
	
	if(mode == 1)										// mode为1时以黑色填充背景
		memset((short*)VideoAddrStart, 0, g_xMax*g_yMax*2);

	p += 54;							// 指向图像内容
	if(fileType != 0x4D42)				// 判断是否BMP文件
	{
		Rewriteletter((g_xMax>>1)-32 ,g_yMax>>1,"无效位图",1,1,RGB(255,255,255),RGB(0,0,0));
		return FALSE;					// 显示BMP位图失败
	}
	else
	{
		if(imageColorBit == 16)					// 判断图片颜色位数是否16位 
		{
			patch = (4 - width * 2 % 4) % 4;	// 补0的个数
			
			for(y=StratY; y<height+StratY; y++)
			{
				for(x=StartX; x<width+StartX; x++)
				{
					if(y < g_yMax && x < g_xMax)
					{
						color = ColorPC(p[1], p[0]);
						setpixel(x ,y ,color);
					}
					p += 2;
				}
				p += patch;						// 跳过每行补上的0
			}
		}
		else
		{
			patch = (4 - (width * 3) % 4) % 4;	// 补0的个数
			
			for(y=StratY; y<height+StratY; y++)
			{
				for(x=StartX; x<width+StartX; x++)
				{
					blue = p[0];
					green = p[1];
					red = p[2];
					p += 3;
				
					if(y < g_yMax && x < g_xMax)
					{
						color = RGB(red ,green ,blue);
						setpixel(x ,y ,color);
					}
				}
				p += patch;						// 跳过每行补上的0
			}
		} // end for "if"
	} // end for "if"
	
	return TRUE;			// 显示BMP位图成功
}

//画线函数
void line(int x0,int y0,int x1,int y1,COLOR mcolor)
{
	int x;
	int y;
	if(x0==x1)
	{	
		for(y=y0;y<=y1;y++)
			setpixel(x0,y,mcolor);
	       	
		return;
	}
	for(x=x0;x<=x1;x++)
	{
		y=(int)(((float)(x-x0))*(y1-y0)/(x1-x0) + y0);
		setpixel(x,y,mcolor);
	}
                for(x=x0;x>=x1;x--)
	{
		y=(int)(((float)(x-x0))*(y1-y0)/(x1-x0) + y0);
		setpixel(x,y,mcolor);
	}
}


//填色函数
void fillrectangle(int x0,int y0,int x1,int y1,COLOR mcolor)
{
	int x,y;
	if (x0 > x1)
	{	
		x = x0;
		x0 = x1;
		x1 = x;
	}
	if (y0 > y1)
	{	
		y = y0;
		y0 = y1;
		y1 = y;
	}
	
	for(x=x0;x<=x1;x++)
		for(y=y0;y<=y1;y++) setpixel(x,y,mcolor);
}

/*
||	函数名:	DrawFrame		画框函数
||	参数1:	int xS,			开始点 X 坐标
||	参数2:	int yS,			开始点 Y 坐标
||	参数3:	int xE,			结束点 X 坐标
||	参数4:	int yE,			结束点 Y 坐标
||	参数5:	int width,		框的厚度
||	参数6:	COLOR mcolor,	框的颜色
||	返回值:	无
*/
void DrawFrame(int xS, int yS, int xE, int yE, int width, COLOR mcolor)
{
	while(width)
	{
		width--;
		
		line(xS + width, yS + width, xE - width, yS + width, mcolor);
		line(xE - width, yS + width, xE - width, yE - width, mcolor);
		line(xE - width, yE - width, xS + width, yE - width, mcolor);
		line(xS + width, yE - width, xS + width, yS + width, mcolor);
	}
}

//取返色函数
void fillreverserectangle(int x0,int y0,int x1,int y1)
{
	int x,y;
	COLOR mcolor;
	
	if (x0 > x1)
	{	
		x = x0;
		x0 = x1;
		x1 = x;
	}
	if (y0 > y1)
	{	
		y = y0;
		y0 = y1;
		y1 = y;
	}
	
	for(x=x0;x<=x1;x++)
	   for(y=y0;y<=y1;y++)
	   {
	   	mcolor = getpixel(x,y);
	    setpixel(x,y,~mcolor);
	   }
}

void ascii_168(int x0,int y0,char *code,int zoom,int pencolor)
{
	char mask[]={0x80,0x40,0x20,0x10,0x8,0x4,0x2,0x1};		// 掩码
	char mat[16];											// 字模数组
	char *charPos;								// 该字模的物理地址
	int offset;									// 该字模在字库中的位置
	int i,j;									// 循环控制变量
	
	offset = (*code) * 16;						// 求出该字模在字库的位置
	charPos = (char *)(asciibufbase + offset);	// 求出字模的物理地址
	
	memcpy(mat, charPos, 16);					// 字模复制到字模数组 mat[] 中
	
	// 在LCD上显示ASCII字符
	for (i=0; i<16; i++)
		for (j=0; j<8; j++)
			if (mask[j%8] & mat[i])
				setpixel(x0+zoom*j, y0-zoom*i, pencolor);
}

//写ASCII字符串函数
int drawascii168(int x0,int y0,char *c,int zoom,int space,int pencolor)             
{    
	int x,y,counter;
	x=x0;
	y=y0;
	counter=0;
	while(*c!='\0')    /*while don't reach to the end of the string*/
	{
		while(x<g_xMax && (*c!='\0'))   /*while the X coordinate don't exceed the scope of X limit*/
		{
			ascii_168(x,y,c,zoom,pencolor);  /*display the character*/
			x+=8*zoom+space;
			c+=1;
			counter+=1;
		}
        	x=0;  /*exchange the next row*/                                              
        	y+=16*zoom;
    }
     return(counter);
}       


void get_hz(int x,int y,char incode[],int zoom,int pencolor)                   
{	 
	char mask[]={0x80,0x40,0x20,0x10,0x8,0x4,0x2,0x1};		// 掩码
	char mat[32];											// 字模数组
	char section,position;						// 区位码(section,区号;position,位号)
	char *chCharPos;							// 该字模的物理地址
	int offset;									// 该字模在字库中的位置
	int i,j,k;									// 循环控制变量

	section = incode[0] - 0xA0;					// 求出区号
	position = incode[1] - 0xA0;				// 求出位号
	offset = (94 * (section - 1) + (position - 1)) * 32;	// 求出该字模在字库的位置
	chCharPos = (char *)(hzbufbase + offset);				// 求出字模的物理地址

	memcpy(mat, chCharPos, 32);					// 字模复制到字模数组 mat[] 中

	// 在LCD上显示汉字
	for (i=0; i<16; i++)
		for (j=0; j<2; j++)
			for (k=0; k<8; k++)
				if (mask[k%8] & mat[2*i+j])
					setpixel(x+zoom*(8*j+k), y-zoom*i,pencolor);
}

 //写汉字函数 
int drawtext(int x,int y,char *c,int zoom,int space,int pencolor)             
{
	int counter = 0;						// 计算已显示汉字的数量
	
	// 如果遇到字符串结束符就结束显示汉字
	while(*c!='\0')
	{
		// 如果汉字坐标不超过LCD界限就显示
		while(x<g_xMax && (*c!='\0'))
		{
			get_hz(x,y,c,zoom,pencolor);  // 在LCD上显示汉字
			x += 16 * zoom + space;		// 下一个汉字的x坐标
			c += 2;							// 指向下一个汉字(一个汉字占两个字节)
			counter += 1;					// 显示汉字计数
		}
			x = 0;							// 换行后的x坐位
			y += 32 * zoom;				// 换行后的y坐位
	}
	
	return(counter);						// 返回已显示汉字数量
}

int drawletter(int x0,int y0,char *c,int zoom,int space,int pencolor)
{
	int x,y,counter,ag = 0;
	x=x0;
	y=y0;
	counter=0;
	while (*c!='\0')
	{
		/*if don't reach to the end of the string display the character in LCD*/
		while (x<g_xMax && (*c!='\0'))
		{
			if ((*c <= 0x7f) && ( *c > 0 ))
			{
				if (ag == 1)
				{
					x++;
					ag = 0;
				}
				ascii_168(x, y-2, c, zoom, pencolor);  /*display the character*/
				x += 8*zoom + space;
				c++;
				counter++;
			}
			else
			{
				if (ag == 0)
				{
					x++;
					ag = 1;
				}
				get_hz(x, y, c, zoom, pencolor);  /*display the Chines character*/
				x += 16*zoom + space;
				c += 2;
				counter++;
			}
		}
        	x=0;                                                
        	y+=32*zoom;
     }
     return counter;	
}



int Rewriteletter(int x0,int y0,char *c,int zoom,int space,int pencolor,int colorin)
{
	int x,y,i,k,counter,ag = 0;
	x=x0;
	y=y0;
	counter=0;
	while (*c!='\0')
	{
		/*if don't reach to the end of the string display the character in LCD*/
		while (x<g_xMax && (*c!='\0'))
		{
			if ((*c <= 0x7f) && ( *c > 0 ))
			{
				if (ag == 1)
				{
					for (i=0; i<(zoom*16); i++)
			  		{
			  			setpixel(x,y-i,colorin);
		 	   		}
					x++;
					ag = 0;
				}
				
				for (i=0; i<(zoom*16); i++)				//填色
			  	{
			  		for (k=0; k<(zoom*8); k++)
			  		{
			  			setpixel(x+k,y-i,colorin);
		 	   		}
		 	   	}
				ascii_168(x, y-2, c, zoom, pencolor);  /*display the character*/
				
				x += 8*zoom;
				for (k=0; k<(zoom*space); k++)				//填色
			  	{
			  		for (i=0; i<(zoom*16); i++)
			  		{
			  			setpixel(x+k,y-i,colorin);
		 	   		}
		 	   	}
				x += space;
				c++;
				counter++;
			}
			else
			{
				if (ag == 0)
				{
					for (i=0; i<(zoom*16); i++)
			  		{
			  			setpixel(x,y-i,colorin);
		 	   		}
					x++;
					ag = 1;
				}
				for (i=0; i<(zoom*16); i++)				//填色
			  	{
			  		for (k=0; k<(zoom*16); k++)
			  		{
			  			setpixel(x+k,y-i,colorin);
		 	   		}
		 	   	}
				
				get_hz(x, y, c, zoom, pencolor);  /*display the Chines character*/
				x += 16*zoom;
				for (k=0; k<(zoom*space); k++)				//填色
			  	{
			  		for (i=0; i<(zoom*16); i++)
			  		{
			  			setpixel(x+k,y-i,colorin);
		 	   		}
		 	   	}
				x += space;
				c += 2;
				counter++;
			}
		}
        	x=0;                                                
        	y+=32*zoom;
     }
     return counter;	
}

/*	c:The written data
/	TextColor: The color of data
/	x_start:开始点x坐标
/	y_start:开始点y坐标
/	x_end:结束点x坐标
/	y_end:结束点y坐标
/	EditID:ID号					
/	'$'代表换行
*/
void PrintTextEdit(char *c,int pencolor,int x_start,int y_start,int x_end,int y_end,int EditID)
{
	int ag = 0;
#ifndef	STN
	int i,j;
#endif	
	
	if (TextEdit_id != EditID)
	{
		TextEdit_x = x_start;
		TextEdit_y = y_start;
		TextEdit_id = EditID;
	} 
	
	while (*c!='\0')
	{
		/*if don't reach to the end of the string display the character in LCD*/
#ifdef	STN
		if(TextEdit_y <= (y_end+16))
		{
			TextEdit_y = y_start;
			fillrectangle(x_start,y_start,x_end,y_end,RGB(140,140,255));
		}	
#else
		if(TextEdit_y < (y_end+32))
		{
			if (mGuiOrientation == GUI_HORIZONTAL)
			{
				for(j = y_start; j >= TextEdit_y;j--)
					for(i = x_start; i < x_end; i++)
					{
						*(short *)(VideoAddrStart+(i*g_yMax+j)*2) = *(short *)(VideoAddrStart+(i*g_yMax+j-16)*2);
					}
			}
			else
			{
				for(j=y_start; j>=TextEdit_y; j--)
					for(i=x_start; i<x_end; i++)
						*(short *)(VideoAddrStart+(((g_yMax-j)*g_xMax+i)<<1)) = *(short *)(VideoAddrStart+(((g_yMax-j+16)*g_xMax+i)<<1));
			}
			TextEdit_y += 16;	
		}
#endif
		
			
		while ( (*c!='\0') && (*c!='$') )
		{
			if ((*c <= 0x7f) && ( *c > 0 ))
			{
				if (TextEdit_x >= (x_end-8))
					goto nextline;
					
				if (ag == 1)
				{
					TextEdit_x++;
					ag = 0;
				}
				ascii_168(TextEdit_x, TextEdit_y-2, c, 1, pencolor);  /*display the Ascii character*/
				TextEdit_x += 8;
				c++;
			}
			else
			{
				if (TextEdit_x >= (x_end-16))
					goto nextline;
				
				if (ag == 0)
				{
					TextEdit_x++;
					ag = 1;
				}
				get_hz(TextEdit_x, TextEdit_y, c, 1, pencolor);  /*display the Chines character*/
				TextEdit_x += 16;
				c += 2;
			}
		}
		
		if ((*c == '$'))
		{
			c++ ; 
nextline:
			TextEdit_y -= 16;
    		TextEdit_x = x_start;		   /* exchange the next row */
		}
		
     }
}


//风格函数
void WIN_WaterTek(char *ch)
{
	clrsrc(RGB(130,130,240));
#ifdef STN
	fillrectangle(0,0,640,30,RGB(0,0,128));
	fillrectangle(0,450,640,480,RGB(0,0,128));
	drawascii168(230,21,"www.watertek.com",1,1,RGB(255,255,255));
	drawletter(15,472,ch,1,0,RGB(255,255,255));

#else
	fillrectangle(0,0,g_xMax,30,RGB(0,0,128));
	fillrectangle(0,g_yMax-30,g_xMax,g_yMax,RGB(0,0,128));
	drawascii168((g_xMax-140)/2,21,"www.watertek.com",1,1,RGB(255,255,255));
	drawletter(15,g_yMax-8,ch,1,0,RGB(255,255,255));
#endif

}

int LcdOrientation(void)
{
	if(mGuiOrientation == GUI_VERTICAL)
	{
		LcdHorizontal();
	}
	else
	{
		LcdVertical();
	}
	return mGuiOrientation;
}

void LcdVertical(void)
{
	mGuiOrientation = GUI_VERTICAL;
	g_xMax = 240;
	g_yMax = 320;
}

void LcdHorizontal(void)
{
	mGuiOrientation = GUI_HORIZONTAL;
	g_xMax = 320;
	g_yMax = 240;
}

//void LcdOrientationInit(void)
//{
//	mGuiOrientation = GUI_VERTICAL;
//	g_xMax = 240;
//	g_yMax = 320;
//}