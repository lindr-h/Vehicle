#include <string.h>
#include <stdarg.h>


#include "key.h"
#include "2410addr.h"
#include "GUI.h"

#define TRUE 	1   
#define FALSE 	0

extern int keyvalue;
extern char phone_num[30];
extern int  phone_index;
extern int a[4];
static volatile   int flag=0;
extern int gTimeIndex;
extern char gTimestr[20];


void __irq KB_LISR(void);
void keyinit(void);
int KeyScan(void);
void LedShow( int key,int i);
void KB_Enable(int flag);



void KBD_Entry()
{
    int i;
   
    keyinit();     
    KB_Enable(TRUE);                      //中断使能函数
}
   

void swap(int b[])
{
    int temp,temp1,temp2;
    temp=b[0];
    temp1=b[1];
    temp2=b[2];
    
    b[1]=temp;
    b[2]=temp1;
    b[3]=temp2;
         
}
void drawtimestr(char* ptimestr)
{
	char tempTimeStr[6];
	char timeStr[20];
	char weekStr[7];
	int i;
	strcpy(tempTimeStr,ptimestr);
	for(i=gTimeIndex;i<5;i++)
	 tempTimeStr[i] = '0';
	sprintf(timeStr,"%c%c:%c%c",tempTimeStr[0],tempTimeStr[1],tempTimeStr[2],tempTimeStr[3]);
	fillrectangle(118,120,160,138,rgb(0,0,255));
	drawascii168(120,138,timeStr,rgb(255,255,0));
	
	if(gTimeIndex>=2) i = gTimeIndex+1;
		else i = gTimeIndex ;
	if(i > 4) i=4;
	if(gTimeIndex <4 ) line(120+i*8,122,120+(i+1)*8,122,rgb(255,0,0));
	
	if(tempTimeStr[4] == '1')  strcpy(weekStr,"星期一");
	else if(tempTimeStr[4] == '2')  strcpy(weekStr,"星期二");
	else if(tempTimeStr[4] == '3')  strcpy(weekStr,"星期三");
	else if(tempTimeStr[4] == '4')   strcpy(weekStr,"星期四");
	else if(tempTimeStr[4] == '5')   strcpy(weekStr,"星期五");
	else if(tempTimeStr[4] == '6')   strcpy(weekStr,"星期六");
	else    strcpy(weekStr,"星期日");
	
	fillrectangle(178,120,230,140,rgb(0,0,255));	
	drawtext(180,138,weekStr,1,0,rgb(255,255,0));			
	if(gTimeIndex == 4) line(180,120,230,120,rgb(255,0,0));
}

void __irq KB_LISR(void)
{
    int value,i,j;

    value=KeyScan();                      //扫描键盘
    
    
    for(i=0;i<3000;i++);
    if(value>9&&value <15) 		//忽略乱码
    {
   	
      rGPGCON = rGPGCON&~(0x300000)|(0x200000);  //设置GPG10=EINT18
      rGPECON=rGPECON&~(0X3FC00)|(0x15400);                //GPE[5:8]=OUTPUT
      rGPEUP=0x3fe0;                      //关闭上拉电阻
      rGPEDAT=rGPEDAT&0xfe1f;     //GPE[5:8]初始为低电平 
   
      rEINTPEND	|=0x40000;                      //中断源请求关闭
      rSRCPND=0x20;                        //中断源请求关闭
      rINTPND=0X20;
	return;
	}
    swap(a);
    a[0]=value;						      //led显示键值    // 
    
    keyvalue = value;
    
    if(!(win_ID == CALL_WIN || win_ID == MESSAGE_WIN ))	/*LedShow(keyvalue,i)*/; 

    //如果目前界面处于GPRS，则应执行if后的语句
   if(win_ID == CALL_WIN || win_ID == MESSAGE_WIN )
   	{
 
 		for(i=0;i<3000;i++);	  	
 	  	//关闭显示
 	  	for(i=0;i<4;i++)  	  	LedShow(15,i);
 	  	//添加电话号码
	   	if(keyvalue>=0&& keyvalue<=9)
	   	  {
  	   		 if(phone_index<15)
  	   		   	   		 phone_num [phone_index++] = '0'+ keyvalue;
	   		  
 	  		 phone_num [phone_index] ='\0';			//添加字符串结尾符
 			 drawascii168(200,148,phone_num,rgb(255,255,0));

   		  }

		if(keyvalue == 15) //删除电话号码
		{
 	   		 if(phone_index>0)
 	   		 {
 		   		 drawascii168(200,148,phone_num,rgb(0,0,255)); 
	      		  	phone_num [--phone_index] ='\0';
	 	   		 drawascii168(200,148,phone_num,rgb(255,255,0)); 
 	   		}
 		}		  	
  
   	}
    //如果目前界面处于设置时间窗口，则应执行if后的语句
   if(win_ID == TIME_CONFIG_WIN )
   	{
 
 		for(i=0;i<3000;i++);	  	
 	  	//关闭显示
 	  	for(i=0;i<4;i++)  	  	LedShow(15,i);
 	  	//添加电话号码
	   	if(keyvalue>=0&& keyvalue<=9)
	   	  {
  	   		 if(gTimeIndex<5)
 	   	   		 gTimestr [gTimeIndex++] = '0'+ keyvalue;
	   		  
 	  		 gTimestr [gTimeIndex] ='\0';			//添加字符串结尾符
 //			 drawascii168(200,148,gTimestr,rgb(255,255,0));

   		  }

		if(keyvalue == 15) //删除电话号码
		{
 	   		 if(gTimeIndex>0)
 	   		 {
//	 	   		 drawascii168(200,148,gTimestr,rgb(0,0,255)); 
	      		  	gTimestr [--gTimeIndex] ='\0';
//	 	   		 drawascii168(200,148,gTimestr,rgb(255,255,0)); 
	 	   	}
 		}
 		drawtimestr(gTimestr);		  	
  
   	}
  	
      rGPGCON = rGPGCON&~(0x300000)|(0x200000);  //设置GPG10=EINT18
      rGPECON=rGPECON&~(0X3FC00)|(0x15400);                //GPE[5:8]=OUTPUT
      rGPEUP=0x3fe0;                      //关闭上拉电阻
      rGPEDAT=rGPEDAT&0xfe1f;     //GPE[5:8]初始为低电平 
   
      rEINTPEND	|=0x40000;                      //中断源请求关闭
      rSRCPND=0x20;                        //中断源请求关闭
      rINTPND=0X20;

     

}


void keyinit(void)
{
    rGPECON=0xfffd57ff;                  //GPE[5:8]=OUTPUT
    rGPEUP=0x1e0;                        //关闭上拉电阻
    rGPEDAT=rGPEDAT&0xfe1f;              //GPE[5:8]初始为低电平
    
    rGPGCON=rGPGCON&~(0x300000)|(0x200000);      //设置GPG10=EINT18
    rGPGUP=rGPGUP|0x400;                   //关闭GPG10上拉电阻 
}

int KeyScan(void)
{
      short a,b,c,d,e,i,j;
      int temp=0;
      rGPEUP=0x1e0;
  

       rGPGCON=rGPGCON&~(0x300000)|(0x0);                    //设置GPG10为输入
       for(j=0;j<10;j++);									 //去抖动
      while((rGPGDAT&(0x400))==0x400)
      {
         ;;
      }
      for(j=0;j<5000;j++);

      
    //  rGPECON=0xfffc03ff;
      
      //第一行检测
      rGPECON=0xfffc07ff;                   //设置GPE5=OUTPUT，GPE[6：8]=INPUT
      //rGPEDAT=rGPEDAT&~(0x1e0)|0x20;                          
      rGPEDAT=rGPEDAT&0xffdf;                        //设置GPE5低电平输出，GPE[6：8]输入
      for(i=0;i<=50;i++);                    //延时去抖动                     
      a=rGPEDAT&(0x100);                    //GPEDAT的GPE8位
      b=rGPEDAT&(0x80);                     //GPEDAT的GPE7位  
      c=rGPEDAT&(0x40);                     //GPEDAT的GPE6位
      d=rGPEDAT&(0x20);                     //GPEDAT的GPE5位
      e=rGPGDAT&(0x400);                      //GPGDAT的GPG10位
     
      if(e==0x0)
      {
          if(((a>>6)+(b>>6)+(c>>6))==0x7)
              temp=1;
          else if(((a>>6)+(b>>6)+(c>>6))==0x6)
              temp=2;
          else if(((a>>6)+(b>>6)+(c>>6))==0x5)
              temp=3;
          else if(((a>>6)+(b>>6)+(c>>6))==0x3)
              temp=15;
      }
      else
      {
          //第二行检测
          rGPECON=0xfffc13ff;                   //设置GPE6=OUTPUT，GPE[5\7\8]=INPUT
          rGPEDAT=rGPEDAT&0xffbf;               //使GPE6输出低电平
          for(i=0;i<=50;i++);                  //延时去抖动
          a=rGPEDAT&(0x100);                    //GPEDAT的GPE8位
          b=rGPEDAT&(0x80);                     //GPEDAT的GPE7位  
          c=rGPEDAT&(0x40);                     //GPEDAT的GPE6位
          d=rGPEDAT&(0x20);                     //GPEDAT的GPE5位
          e=rGPGDAT&(0x400);                       //GPGDAT的GPG10位
          
          if(e==0x0) 
          {
              if(((a>>6)+(b>>6)+(d>>5))==0x6)    
                 temp=4;
              else if(((a>>6)+(b>>6)+(d>>5))==0x7)
                 temp=5;
              else if(((a>>6)+(b>>6)+(d>>5))==0x5) 
                 temp=6;
              else if(((a>>6)+(b>>6)+(d>>5))==0x3)
                 temp=14;
          }
          else
          {
              //第三行检测
              rGPECON=0xfffc43ff;                   //设置GPE7=OUTPUT，GPE[5\6\8]=INPUT
              rGPEDAT=rGPEDAT&0xff7f;                        //使GPE7输出低电平
              for(i=0;i<=50;i++);                  //延时去抖动

              a=rGPEDAT&(0x100);                    //GPEDAT的GPE8位
              b=rGPEDAT&(0x80);                     //GPEDAT的GPE7位  
              c=rGPEDAT&(0x40);                     //GPEDAT的GPE6位
              d=rGPEDAT&(0x20);                     //GPEDAT的GPE5位
              e=rGPGDAT&(0x400);
              
              if(e==0x0) 
              {
                   if(((a>>6)+(c>>5)+(d>>5))==0x6)     
                      temp=7;
                   else if(((a>>6)+(c>>5)+(d>>5))==0x5)
                      temp=8;
                   else if(((a>>6)+(c>>5)+(d>>5))==0x7) 
                      temp=9;
                   else if(((a>>6)+(c>>5)+(d>>5))==0x3) 
                      temp=13;
              }
              else
              {
                   //第四行检测
                   rGPECON=0xfffd03ff;            //设置GPE8=OUTPUT，GPE[5:7]=INPUT
                   rGPEDAT=rGPEDAT&0xfeff;                //使GPE8输出低电平
   //                for(i=0;i<=50;i++);                  //延时去抖动

                   a=rGPEDAT&(0x100);                    //GPEDAT的GPE8位
                   b=rGPEDAT&(0x80);                     //GPEDAT的GPE7位  
                   c=rGPEDAT&(0x40);                     //GPEDAT的GPE6位
                   d=rGPEDAT&(0x20);                     //GPEDAT的GPE5位
                   e=rGPGDAT&(0x400);
                  
                   if(((b>>5)+(c>>5)+(d>>5))==0x6)    
                        temp=10;
                   else if(((b>>5)+(c>>5)+(d>>5))==0x5)
                        temp=0;
                   else if(((b>>5)+(c>>5)+(d>>5))==0x3) 
                        temp=11;
                   else if(((b>>5)+(c>>5)+(d>>5))==0x7) 
                        temp=12;
                    
              }
          }        
      }
      
    /*  rEINTPEND=0x400;                      //中断源请求关闭
      rSRCPND=0x20;                        //中断源请求关闭
      rINTPND=0X20;
      rGPGCON = rGPGCON&~(0x30)|(0x20);  //设置GPG2=EINT10
      rGPECON=0xfffd57ff;                //GPE[5:8]=OUTPUT
      rGPEUP=0x1e0;                      //关闭上拉电阻
      rGPEDAT=0x1e0;                     //GPE[5:8]初始为高电平
     */
      for(j=0;j<10;j++);
      while((rGPGDAT&(0x400))!=0x400)
      {
         ;;
      }
      for(j=0;j<1000;j++);
      while((rGPGDAT&(0x400))!=0x400)
      {
         ;;
      }
      return temp;
}


void LedShow( int key,int i)
{
    int  j;
    int table[16]={0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};
   //unsigned short dis_buf[3]={0x3,0x2,0x1,0x0}
   
   rGPHCON=rGPHCON&~(0x30000)|0x10000;            //设置点，不亮
   rGPHUP=rGPHUP|0x100;
   rGPHDAT=rGPHDAT&~(0x100)|0x0; 
   
   rGPECON=rGPECON&~(0xffc0000)|(0x5540000);  //设置GPE[9：13]=OUTPUT
   rGPEUP|=0x3e00;
   rGPGCON=rGPGCON&~(0x30)|(0x10);             //设置GPG2为OUTPUT
   rGPGUP|=0x4;
   
     rGPEDAT=rGPEDAT&~(0x3C00)|(table[15]<<10);
   
   switch(i)
   {
   case 0:
      rGPGDAT=rGPGDAT|(1<<2);                   //设置数码管的位选GPG2=1,A
      rGPEDAT=rGPEDAT|(1<<9);                   //设置数码管的位选GPE9=1,B
      break;
   case 1:
      rGPGDAT=rGPGDAT&~(1<<2);                   //设置数码管的位选GPG2=0,A
      rGPEDAT=rGPEDAT|(1<<9);                   //设置数码管的位选GPE9=1,B
      break;
   case 2:
      rGPGDAT=rGPGDAT|(1<<2);                   //设置数码管的位选GPG2=1,A
      rGPEDAT=rGPEDAT&~(1<<9);                   //设置数码管的位选GPE9=0,B
      break;
   case 3:
      rGPGDAT=rGPGDAT&~(1<<2);                   //设置数码管的位选GPG2=0,A
      rGPEDAT=rGPEDAT&~(1<<9);                   //设置数码管的位选GPE9=0,B
      break;
      }
  // if((key>0x0)&&(key<=0xf))

   rGPEDAT=rGPEDAT&~(0x3C00)|(table[15]<<10);
   for(j=0;j<50;j++);
    rGPEDAT=rGPEDAT&~(0x3C00)|(table[key]<<10);
   for(j=0;j<=1000;j++);  
   
  
 //   rGPGCON=rGPGCON&~(0x30);             //设置GPG2为INPUT
 //   rGPGUP|=0x4;
  //  rGPECON=rGPECON&~(0xffc0000)|(0x5540000);  //设置GPE[9]=INPUT
  //  rGPEUP|=0x3e00;
   }


void KB_Enable(int flag)
{
     int temp;
     if(flag == TRUE)
     {
           
            temp = rGPGCON;
 	        rGPGCON = temp&~(0x300000)|(0x200000);        //设置GPG10=EINT18
 	        temp = rGPGUP;
 	        rGPGUP = temp|0x400;                    //关闭GPG10上拉电阻 
 	        
            rSRCPND=0x20;                         //中断源请求 
	        rINTMSK&= ~(BIT_EINT8_23);	          //中断源使能
            temp=rEXTINT2;
            rEXTINT2&=0xfffff8ff;                 //使EINT18为低电平触发
            rEINTPEND=0X40000;                      //外部中断源请求
            rEINTMASK=0xfbfff8;                   //使能外部中断EINT18
      }
      else
      {
	        rINTMSK&= BIT_EINT8_23;
            
      }		
      	
}


