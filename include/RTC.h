#ifndef RTC_H
#define RTC_H


typedef struct
{
	int year;
	int month;
	int day;
	int weekday;
	int hour;
	int min;
	int sec;
	
}TIME;



void resetRTC(void);
void readRTC(TIME* ptime);
void writeRTC(TIME* ptime);

#endif	