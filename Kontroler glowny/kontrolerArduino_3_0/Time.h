// Time.h

#ifndef _TIME_h
#define _TIME_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class TimeClass
{
 protected:
     char yearSaved[3];
     char monthSaved[3];
     char daySaved[3];
     char hourSaved[3];
     char minuteSaved[3];
     char secondSaved[3];

 public:
	void init();
    void timeStamp(char* buffer);         //make timestamp in dec
    void getNewTime();                    //get time from RTC
    void hexTime(char* buffer);           //make time in HEX format <YYDDMMHHIISS>
    void setNewTime(char* newTime);       //set new time to RTC module
    void getYear(char* buffer);
    void getTimeStamp(char* buffer);
};

extern TimeClass Time;

#endif

