//===================================================================
// File Name : 2410lib.h
// Function  : S3C2410 
// Program   : Shin, On Pil (SOP)
// Date      : May 14, 2002
// Version   : 0.0
// History
//   0.0 : Programming start (February 20,2002) -> SOP
//         Mar.29.2002:purnnamu: For POWEROFF_wake_up, the START... label is added
//===================================================================

#ifndef __2410lib_h__
#define __2410lib_h__

#ifdef __cplusplus
extern "C" {
#endif

#define DebugOut Uart_Printf

#define min(x1,x2) (((x1)<(x2))? (x1):(x2))
#define max(x1,x2) (((x1)>(x2))? (x1):(x2))

#define ONESEC0 (62500)	             //16us resolution, max 1.04 sec
#define ONESEC1 (31250)	             //32us resolution, max 2.09 sec
#define ONESEC2 (15625)	             //64us resolution, max 4.19 sec
#define ONESEC3 (7812)	             //128us resolution, max 8.38 sec
#define ONESEC4 (PCLK/128/(0xff+1))  //@60Mhz, 128*4us resolution, max 32.53 sec

#define NULL 0
#define ERROR		(-1)

#define EnterPWDN(clkcon) ((void (*)(int))0x20)(clkcon)



typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned int   INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   int   INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned long  INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   long  INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */



void StartPointAfterPowerOffWakeUp(void); //purnnamu:Mar.29.2002

// 2410lib.c
void Delay(int time);              //Watchdog Timer is used.

void *malloc(unsigned nbyte); 
void free(void *pt);

void Port_Init(void);

void Timer_Start(int divider);    //Watchdog Timer is used.
int  Timer_Stop(void);            //Watchdog Timer is used.

void Led_Display(int data);
void Led1_On_or_Off(int flag);
void Led4_On_or_Off(int flag);


void ChangeMPllValue(int m,int p,int s);
void ChangeClockDivider(int hdivn,int pdivn);
void ChangeUPllValue(int m,int p,int s);

#ifdef __cplusplus
}
#endif

#endif  //__2410lib_h__