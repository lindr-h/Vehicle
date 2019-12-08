#ifndef __IIC_H__
#define __IIC_H__

static unsigned char _iicData[256];
static volatile int _iicDataCount;
static volatile int _iicStatus;
static volatile int _iicMode;
static int _iicPt;

#define WRDATA      (1)
#define POLLACK     (2)
#define RDDATA      (3)
#define SETRDADDR   (4)

void Wr24C02(unsigned int slvAddr,unsigned int addr,unsigned char data);    
void Rd24C02(unsigned int slvAddr,unsigned int addr,unsigned char *data); 
void IicPoll(void);
void Run_IicPoll(void);
void DelayTime(int time);
void IIC_read(unsigned int addr ,unsigned char* pdata);

#endif

