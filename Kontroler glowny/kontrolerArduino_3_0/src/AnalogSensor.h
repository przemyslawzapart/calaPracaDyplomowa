// AnalogSensor.h

#ifndef _ANALOGSENSOR_h
#define _ANALOGSENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DigitalSensor.h"

class AnalogSensorClass //: public DigitalSensorClass
{
 protected:
	 char name[20];
	 int value;
	 char unit[20];
	 int minValue;
	 int maxValue;
	 int range;
	 unsigned int pinPosition;
	 unsigned int id;

 public:
	void init();
	AnalogSensorClass();
	AnalogSensorClass(int pinPosition);
	void getValuesFromEeprom();
	void changeValues(char* name, char* unit, int min, int max, int range);
	char getSettingsValues(char* buffer);
	void measurement();
	void getFlaotValue(char* data);
	int  getValue();
	void show();
	bool getState();

};

extern AnalogSensorClass AnalogSensor;

#endif

//nie zapomnij o destruktorach!!!!!!