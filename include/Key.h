#ifndef	KEY_H
#define KEY_H

#define TRUE 	1   
#define FALSE 	0

//extern int a[4];
void __irq KB_LISR(void);
void keyinit(void);
 int KeyScan(void);
void LedShow( int key,int i);
void KB_Enable(int flag);
void KBD_Entry(void);

#endif
