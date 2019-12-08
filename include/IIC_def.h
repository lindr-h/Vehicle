#ifndef IIC_def_H
#define IIC_def_H

#define WRDATA      (1)
#define POLLACK     (2)
#define RDDATA      (3)
#define SETRDADDR   (4)

void IIC_write(unsigned int addr ,unsigned char data);
void IIC_read(unsigned int addr ,unsigned char* pdata);

void Wr24C02(unsigned int slvAddr,unsigned int addr,unsigned char data);    
void Rd24C02(unsigned int slvAddr,unsigned int addr,unsigned char *data); 
void IicPoll(void);
void Run_IicPoll(void);
void DelayTime(int time);   

#endif

