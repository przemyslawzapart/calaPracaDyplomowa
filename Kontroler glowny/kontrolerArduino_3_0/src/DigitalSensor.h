// DigitalSensor.h

#ifndef _DIGITALSENSOR_h
#define _DIGITALSENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif



class DigitalSensorClass
{
 protected:
	 unsigned int id;				//  id sendsor
	 int pinPosition;		// digital pin
	 char name[15];			// name of senor
	 bool value;			//??
	 bool previousValue;	//??
	 bool errorState;		//current error state
	 bool reset;			//reset
	 unsigned long resetTimer;
	 int contactType;		// NC / NO
	 

 public:
	void init();
	DigitalSensorClass();
	DigitalSensorClass(int pinPosition);
	void show();
	
	void setState(bool state);
	bool getState();
	char* getName();
	int getContactType();
	
	void getValuesFromEeprom();
	void changeValue(char* name, int contact);
	void getFromEeprom(int address, char* data);
	void getSettingsValues(char* buffer);

	bool getValue();
	void checkValue();
	void SetAlarm(bool state);
	void resetSensor();



};

//extern DigitalSensorClass DigitalSensor;

#endif

