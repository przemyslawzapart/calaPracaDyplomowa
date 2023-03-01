

#include "AnalogSensor.h"
#include "Global.h"
#include<EEPROM.h>
#include "Arduino.h"

#define EEPROM_START_ANALOG     	1400

int value = 0;

void AnalogSensorClass::init(){}
AnalogSensorClass::AnalogSensorClass() {}
AnalogSensorClass::AnalogSensorClass(int pinPosition) {
	Serial.begin(19200);
	static  int _id = 0;
	this->pinPosition = pinPosition;
	this->id = _id++;
	strcpy(this->name,"");
	strcpy(this->unit, "");
}

void AnalogSensorClass::getValuesFromEeprom() {
	//int i = id;
	int adr = EEPROM_START_ANALOG + (id * 30);
	Global.getFromEeprom(adr, name);
	adr += 15;
	Global.getFromEeprom(adr, unit);
	adr += 5;
	minValue = EEPROM.read(adr);
	adr += 3;
	maxValue = EEPROM.read(adr);
	adr += 3;
	range = EEPROM.read(adr);
}
void AnalogSensorClass::changeValues(char *_name, char *_unit, int _min, int _max, int _range) {
	int adr = EEPROM_START_ANALOG + (id * 30);
	Global.saveToEprom(adr, _name);
	adr += 15;
	Global.saveToEprom(adr, _unit);
	adr += 5;
	EEPROM.update(adr, _min);
	adr += 3;
	EEPROM.update(adr, _max);
	adr += 3;
	EEPROM.update(adr, _range);

	getValuesFromEeprom();
}
char AnalogSensorClass::getSettingsValues(char *buffer) {
	
	char buf[10];
	strcpy(buffer, "@/");

	sprintf(buf, "%d", id);
	strcat(buffer, buf);
	strcat(buffer, "/");

	strcat(buffer, name);
	strcat(buffer, "/");

	strcat(buffer, unit);
	strcat(buffer, "/");
	
	itoa(minValue, buf, 10);
	strcat(buffer, buf);
	strcat(buffer, "/");

	itoa(maxValue, buf, 10);
	strcat(buffer, buf);
	strcat(buffer, "/");
	itoa(range, buf, 10);

	strcat(buffer, buf);
	strcat(buffer, "/");
	strcat(buffer, "*");
	
}
void AnalogSensorClass::measurement() {
	int sensorValue = analogRead(pinPosition);
	value = (sensorValue*10) * (5.0 / 1023);	
}

void AnalogSensorClass::getFlaotValue(char *data) {
	int pierwsza = value / 10;
	int druga = value%10 ;
	sprintf(data, "%d.%d", pierwsza, druga);

}

int  AnalogSensorClass::getValue() {
	return value;
}
void AnalogSensorClass::show() {
	Serial.print("id : ");
	Serial.print(id);
	Serial.print(", pin : ");
	Serial.print(pinPosition);
	Serial.print(" , name : ");
	Serial.print(name);
	Serial.print(" , unit : ");
	Serial.print(unit);
	Serial.print(", min : ");
	Serial.print(minValue);
	Serial.print(", max : ");
	Serial.print(maxValue);
	Serial.print(", range : ");
	Serial.println(range);

}
bool AnalogSensorClass::getState() {
	if (value > (minValue*10) && value < (maxValue*10))
		return false;
	else 
		return true;
}
AnalogSensorClass AnalogSensor;

