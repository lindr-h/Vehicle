
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "gps.h"
#include "2410addr.h"

#include <string.h>
#include <stdarg.h>
#include "def.h"

//#include ""
#define GOOD_PARSE       0
#define BADID_PARSE      1
#define BADLEN_PARSE     2
#define MAX_AS1_MESSAGE  4
#define MAX_MID_MESSAGE  41
#define MAX_SC_MESSAGE   13
#define MAX_EC_MESSAGE   6
#define INFO_DGPS        0x02
#define INFO_2D          0x04
#define INFO_ALTSET      0x08
#define INFO_FILTERED    0x10
#define MAX_LONG         2147483648.  //2**3


static short parsed;
char *pbuf;
static char timestring[20];
char diop='.';
extern POSITION Pos;
extern VELOCITY Vel;
//static int a,b,c;
char outbuf[OUTBUFSIZE];

#define BYTESWAP       //DOS machines,receive order of bytes as they come through the serial port
#ifdef BYTESWAP


/**************************字符交换*******************************************************************/
int GetInt(float f,int n)
{
    int temp=1,i;
    int a,b,c,m=10;
    for(i=1;i<n+1;i++)
    {
       temp *=m;
    }
    a=(int)f;
    b=(int)(f*temp);
    c=b%temp;
    return c;
}


short bGetShort(unsigned char *bp)
{
    short outval;
    unsigned char *optr;
    
    optr=(unsigned char *)&outval+1;
    *optr--=*bp++;
    *optr=*bp;
    return outval;
}

long bgetlong (unsigned char *bp)
{
    long outval;
    unsigned char *optr;
    
    optr=(unsigned char *)&outval+3;
    *optr--=*bp++;
    *optr--=*bp++;
    *optr--=*bp++;
    *optr=*bp;
    return outval;
}

unsigned long bGetULong(unsigned char *bp)
{
    unsigned long outval;
    unsigned char *optr;
     
    optr=(unsigned char *)&outval+3;
    *optr--=*bp++;
    *optr--=*bp++;
    *optr--=*bp++;
    *optr=*bp;
    return outval;
}

float bGetSingle(unsigned char *bp)
{
    float outval;
    unsigned char *optr;
    
    optr=(unsigned char *)&outval+3;
    *optr--=*bp++;
    *optr--=*bp++;
    *optr--=*bp++;
    *optr=*bp;
    return outval;
}

double bGetDouble(unsigned char *bp)
{
    double outval;
    unsigned char *optr;
    
    optr=(unsigned char *)&outval+7;
    *optr--=*bp++;
    *optr--=*bp++;
    *optr--=*bp++;
    *optr--=*bp++;
    *optr--=*bp++;
    *optr--=*bp++;
    *optr--=*bp++;
    *optr=*bp;
    return outval;
}

#else //not BYTESWAP
#define bGetShort(bp)  (*(short*)bp)
#define bgetlong(bp)   (*(long*)bp)
#define bGetULong(bp)  (*(unsigned long*)bp)
#define bGetSingle(bp)  (*(float*)bp)
#define bGetDouble(bp) (*(double*)bp)
#endif

/****************************时间输出格式***********************/

char *dayname[7]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
char *show_time(float time_of_week)
{
   short days,hours,minutes;
   short seconds;
   double tow;
   //static char timestring[20];
   
   if(time_of_week==-1.0)
   {
      sprintf(timestring,"<No time yet>");
    //  //SDC_Put_String(pbuf,&port0)   ; 
   }
   else if((time_of_week>=604800.0)||(time_of_week<0.0))
   {
      sprintf(timestring," <Bad time> "); 
    //  //SDC_Put_String(pbuf,&port0)   ; 
   }
   else
   {
      tow=time_of_week;
      days=(short)(tow/86400.0);
      hours=(short)fmod(tow/3600.0,24.0)+8;
      minutes=(short)fmod(tow/60.0,60.0);
      seconds=(short)fmod(tow,60.0);
      sprintf(timestring,"%s %2d:%2d:%2d",dayname[days],hours,minutes,seconds);
     // //SDC_Put_String(pbuf,&port1)   ; 
     
   }
   return timestring;
}

/*******************************rpt_0x?**********************/
short rpt_0x4A(TSIPPKT *rpt,float *lat,float *lon,float *alt,float *clock_bias,float *time_of_fix)
{
    unsigned char *buf;
    buf=rpt->buf;
      
    if(rpt->len!=20)return FALSE;
    *lat=bGetSingle(buf);
    *lon=bGetSingle(&buf[4]);
    *alt=bGetSingle(&buf[8]);
    *clock_bias=bGetSingle(&buf[12]);
    *time_of_fix=bGetSingle(&buf[16]); 
    return TRUE;
}


short rpt_0x4A_2(TSIPPKT *rpt,float *alt,float *dummy,unsigned char *alt_flag)
{
    unsigned char *buf;
    buf=rpt->buf;
    
    if(rpt->len!=9) return FALSE;
    *alt=bGetSingle(buf);
    *dummy=bGetSingle(&buf[4]);
    *alt_flag=buf[8];
    return TRUE;
}


short rpt_0x56(TSIPPKT *rpt,float vel_ENU[3],float *freq_offset,float *time_of_fix)
{
    unsigned char *buf;
    buf=rpt->buf;
    
    if(rpt->len!=20)return FALSE;
    vel_ENU[0]=bGetSingle(buf);
    vel_ENU[1]=bGetSingle(&buf[4]);
    vel_ENU[2]=bGetSingle(&buf[8]);
    *freq_offset=bGetSingle(&buf[12]);
    *time_of_fix=bGetSingle(&buf[16]);
    return TRUE;
}

        


/************************report*************************/


static void rpt_single_lla_position(TSIPPKT *rpt)
{ 
   short lat_deg,lon_deg;
   float lat,lon,alt,clock_bias,time_of_fix;
   double lat_min,lon_min;
   unsigned char north_south,east_west;
   char *p;
   int i;
   
   
   if(!rpt_0x4A(rpt,&lat,&lon,&alt,&clock_bias,&time_of_fix))
   {
      parsed=BADLEN_PARSE;
      return;
   } 
   lat *=(float)R2D;
   north_south='N';
   if(lat<0.0)
   {
       north_south='S';
       lat=-lat;
   }
   
   Pos.latdir=north_south;
   sprintf(Pos.lat,"%4d%c%3d",(int)lat,diop,GetInt(lat,3));
   
   lat_deg=(short)lat; 
   lat_min=(lat-lat_deg)*60.0;
   lon  *=(float)R2D;
   east_west='E';
   if(lon<0.0)
   {
       east_west='W';
       lon=-lon;
   }
   
   Pos.londir=east_west;
   sprintf(Pos.lon,"%4d%c%3d",(int)lon,diop,GetInt(lon,3));
   
   lon_deg=(short)lon;
   lon_min=(lon-lon_deg)*60.0;
   p=show_time(time_of_fix);
   p[18]='\0';
   Pos.altdir='U';
   sprintf(Pos.alt,"%8d%c%3d",(int)alt,diop,GetInt(alt,3));
   
   for(i=0;i<100;i++);
  // sprintf(pbuf,"\nSLLA:%4d:%4d%c%3d %c%5d:%4d%c%3d %c%8d%c%3d ",lat_deg,(int)lat_min,diop,GetInt(lat_min,3),
        //   north_south,lon_deg,(int)lon_min,diop,GetInt(lon_min,3),east_west,(int)alt,diop,GetInt(alt,2));
  // for(i=0;i<100;i++);
   
   
}   
    


static void rpt_ENU_velocity(TSIPPKT *rpt)
{
   float ENU_vel[3],fre_offset,time_of_fix;
   char *p;
   int i;
   unsigned char east_west,north_south,up_down;
   
   if(!rpt_0x56(rpt,ENU_vel,&fre_offset,&time_of_fix))
   {
      parsed=BADLEN_PARSE;
      return;
   }
   p=show_time(time_of_fix); 
   east_west='E';
   if(ENU_vel[0]<0.0)
   {
       east_west='W';
       ENU_vel[0]=-ENU_vel[0];
   }
   Vel.eastdir=east_west;
   sprintf(Vel.evel,"%8d%c%3d",(int)ENU_vel[0],diop,GetInt(ENU_vel[0],3));
   
   north_south='N';
   if(ENU_vel[1]<0.0)
   {
       north_south='S';
       ENU_vel[1]=-ENU_vel[1];
   }
   Vel.northdir=north_south;
   sprintf(Vel.nvel,"%8d%c%3d",(int)ENU_vel[1],diop,GetInt(ENU_vel[1],3));
   
   up_down='U';
   if(ENU_vel[2]<0.0)
   {
       up_down='D';
       ENU_vel[2]=-ENU_vel[2];
   }
   Vel.updir=up_down;
   sprintf(Vel.uvel,"%8d%c%3d",(int)ENU_vel[2],diop,GetInt(ENU_vel[2],2));
   
               
   for(i=0;i<100;i++);
   //SDC_Put_String(pbuf,&port0)   ;   
}



/****************************rpt_packet*********************************/

void rpt_packet(TSIPPKT *rpt,char *obuf)
{
   
   pbuf=obuf;
   parsed=GOOD_PARSE;
   
  // SDC_Print_String();
   switch(rpt->code)
   {
   
    case 0x4A:
       if(rpt->len==20)
       {
          rpt_single_lla_position(rpt);
       }
       break;
       
    case 0x56:
       rpt_ENU_velocity(rpt);
       break;
   
    default:
       parsed=BADID_PARSE;
       break;
   }
 

}