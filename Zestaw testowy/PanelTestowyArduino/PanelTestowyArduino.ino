/*
 Name:		PanelTestowyArduino.ino
 Created:	05.01.2023 21:25:20
 Author:	Przremysław Zapart
*/

#include <DigiPotX9Cxxx.h>

DigiPot potentiometr_1(3, 4, 2);
DigiPot potentiometr_2(6, 7, 5);//ok
DigiPot potentiometr_3(49, 50, 48);
DigiPot potentiometr_4(52, 53, 51);

DigiPot potentiometrArray[] = { potentiometr_1, potentiometr_2, potentiometr_3, potentiometr_4 };

int digitalIn[] = { 8,9,10,11,12,13,22,23,24,25,26,27,28,29,30,31 };
int digitalOut[] = { 32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47 };

unsigned int digitalDataIn = 0;
unsigned int digitalDataOut = 0;

unsigned long timer = 0;
unsigned long timerSendData = 0;
int analogValue[] = { 0,0,0,0 };

char data[35];

int rpmPin = 14;
unsigned long rpmTimer = 0;
unsigned long rpmFrequency = 1000; // 600 - ,500-540,400-720, 300-,200-960,100-2640, 200-1360, 50-4950
bool rpmState = false;
bool rpmSendValue = false;


void setup() {
	Serial.begin(19200);
	Serial2.begin(19200);
	Serial2.println("start");
	for (size_t i = 0; i < sizeof(digitalIn) / sizeof(digitalIn[0]); i++)
	{
		pinMode(digitalIn[i], INPUT_PULLUP);
		pinMode(digitalOut[i], OUTPUT);
		digitalWrite(digitalOut[i], LOW);
	}

	pinMode(rpmPin, OUTPUT);
	digitalWrite(rpmPin, HIGH);
}


void loop() {

	if (millis() - timerSendData > 100) {//send data every 100ms
		timerSendData = millis();
		makeDataString();
	}
	if (micros() - rpmTimer > rpmFrequency && rpmSendValue) {
		rpmTimer = micros();
		digitalWrite(rpmPin, rpmState);
		if (rpmState)
			rpmState = false;
		else
			rpmState = true;
	}

}
void makeDataString() {
	char buffer[20];
	getDigitalValue();
	sprintf(buffer, "#%x/%x/", digitalDataIn, digitalDataOut);
	int len = strlen(buffer);
	char buf[5];
	sprintf(buf, "%02x*", len + 3);
	strcat(buffer, buf);
	Serial.println(buffer);
}
void checkRecivedData(char* data) {
	//#ffff/aa/bb/cc/dd/cs*
	Serial2.println(data);
	char split[] = "#/*";
	char* value;
	value = strtok(data, split);
	setDigitalValue(value);
	value = strtok(NULL, split);
	setDigitalPotentiometr(0, value);
	value = strtok(NULL, split);
	setDigitalPotentiometr(1, value);
	value = strtok(NULL, split);
	setDigitalPotentiometr(2, value);
	value = strtok(NULL, split);
	setDigitalPotentiometr(3, value);
	value = strtok(NULL, split);
	rpmFrequency = atoi(value);
	if (rpmFrequency == 0)
		rpmSendValue = false;
	else
		rpmSendValue = true;

}
void getDigitalValue() {
	for (size_t i = sizeof(digitalIn) / sizeof(digitalIn[0]); i > 0; i--)
	{
		int pinPosition = i - 1;
		bool state = digitalRead(digitalIn[pinPosition]);

		if (state) {
			digitalDataIn |= (1 << pinPosition);
		}
		else {
			digitalDataIn &= ~(1 << pinPosition);
		}
	}
	for (size_t i = sizeof(digitalOut) / sizeof(digitalOut[0]); i > 0; i--)
	{
		int pinPosition = i - 1;
		bool state = digitalRead(digitalOut[pinPosition]);

		if (state) {
			digitalDataOut |= (1 << pinPosition);
		}
		else {
			digitalDataOut &= ~(1 << pinPosition);
		}
	}
}
void setDigitalValue(const char* data) {
	unsigned int value = (unsigned int)strtol(data, NULL, 2);
	for (size_t i = 0; i < sizeof(digitalOut) / sizeof(digitalOut[0]); i++)
	{
		if ((value & (1 << i)) != 0)
			digitalWrite(digitalOut[i], HIGH);
		else
			digitalWrite(digitalOut[i], LOW);
	}
}
void setDigitalPotentiometr(uint8_t position, char* value) {
	unsigned int intValue = (unsigned int)strtol(value, NULL, 10);

	int ValueToChange = intValue - analogValue[position];
	analogValue[position] += ValueToChange;

	if (ValueToChange == 0)
		return;
	if (ValueToChange < 0) {
		ValueToChange *= (-1);
		potentiometrArray[position].decrease(ValueToChange);
	}

	else
		potentiometrArray[position].increase(ValueToChange);
}

void serialEvent() {
	static String data = "";
	while (Serial.available()) {
		char c = (char)Serial.read();
		data += c;
		if (c == '*') {
			data.trim();
			const char* dataChar = data.c_str();
			checkRecivedData((char*)dataChar);
			data = "";
		}
	}
}