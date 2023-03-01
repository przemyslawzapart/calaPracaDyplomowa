// Global.h

#ifndef _GLOBAL_h
#define _GLOBAL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class GlobalClass
{
 protected:


 public:
	void init();
	void saveToEprom(int address, const char* data);
	void getFromEeprom(int address, char* data);
	bool getData(const char* data, char* buffer, int start, int len);
};

extern GlobalClass Global;

#endif

