/*
 Name:		kontrolerArduino_3_0.ino
 Created:	05.01.2023 22:36:43
 Author:	Przemyslaw Zapart
*/


#include <EEPROM.h>
#include <DS1307.h>
#include <Wire.h>
#include <SD.h>

#include "Time.h"
#include "src/AnalogSensor.h"
#include "src/DigitalSensor.h"
#include "src/Global.h"



#define SEND_DATA					500	//time to send data to controler in ms
#define EEPROM_START_DIGITAL_IN		200
#define EEPROM_START_DIGITAL_OUTPUT 520
#define EEPROM_START_ENGINE_STATE	840
#define EEPROM_START_ANALOG     	1400

#define EEPROM_DIGITAL_SIZE			20
#define EEPROM_ANALOG_SIZE			30

#define EEPROM_ADR_AMOUNT_OF_THEET	3
#define EEPROM_AMOUNT_OF_THEET		100

#define EEPROM_ADR_SERIAL_NUMBER 	20
#define EEPROM_SERIAL_NUMBER 		"900000000"
#define EEPROM_ADR_SOFTWARE_VERSION	40

#define SYSTEM_PRESURE			0
#define OIL_PRESURE				1
#define ENGINE_TEMPERATURE_LOW	2
#define ENGINE_TEMPERATURE_HIGH	3
#define FLOW_RAW_WATER			4
#define FUEL_LEVEL				5
#define FUEL_VALVE_CLOSE		6
#define FUEL_TANK_LEAK			7
#define CHARGER_1_FAIL			8
#define CHARGER_2_FAIL			9
#define STARTER_1_FAIL			10
#define STARTER_2_FAIL			11
#define USER_1					12
#define USER_2					13
#define USER_3					14
#define USER_4					15

#define STARTER_1				0
#define STARTER_2				1
#define STOP					2
#define ERROR					3
#define HEATING					4


//define engine state
#define CONTROLER_OUT			0
#define ENGINE_READY			1
#define ENGINE_STARTING			2
#define ENGINE_RUNNING			3
#define ENGINE_ERROR			4
#define BATTERY1_FAIL			5
#define BATTERY2_FAIL			6
#define ANALOG3_FAIL			7
#define ANALOG4_FAIL			8

#define BATTERY_1				0
#define BATTERY_2				1


TimeClass time;

DigitalSensorClass digitalSensorArray[] = { {32},{33},{34},{35},{36},{37},{38},{39},{40},{41},{42},{43},{44},{45},{ 46 }, {47} };//8,9,10,11,12,13,22,23,24,25,26,27,28,29,30,31
//DigitalSensorClass digitalSensorArray[] = { {32},{33},{34},{35},{36},{37},{38},{39},{40},{41},{42},{43},{44},{45},{ 46 }, {47} };//8,9,10,11,12,13,22,23,24,25,26,27,28,29,30,31
//DigitalSensorClass digitalSensorArrayRunning[] = { {32},{33},{34},{35},{36},{37},{38},{39},{40},{41},{42},{43},{44},{45},{ 46 }, {47} };//8,9,10,11,12,13,22,23,24,25,26,27,28,29,30,31
AnalogSensorClass analogSensorArray[] = { {A4}, {A4}, {A4}, {A4} };

uint8_t digitalOutArray[] = { 8,9,10,11,12,13,22,23,24,25,26,27,28,29,30,31 };

DigitalSensorClass digitalSensorArrayOutput[] = { {8},{9},{10},{11},{12},{13},{22},{23},{24},{25},{26},{27},{28},{29},{30}, {31} };//8,9,10,11,12,13,22,23,24,25,26,27,28,29,30,31

const char* softwareVersion = "1.002";
unsigned int amountOfStarts = 6;


bool flagHandStarting = false;		//sprawdza czy przycisk hand start został pryciśnięty
bool flagBattery = true;			//true - bateria 1 , false - bateria 2


uint8_t muxPinPosition[] = { 4,5,6,7 };
uint8_t muxAnalogPin = A4;

unsigned long timer = 0;
//----------
bool keyOffState = false;
bool keyHandState = false;
bool keyAutoState = false;

bool keyStart = false;
bool keyStop = false;
bool keyReset = false;

//--------------
unsigned int digitalInputState = 0;
unsigned int digitalOutputState = 0;
unsigned int engineState = 0;


//-------------------

bool resetFlag = false;
bool resetState = false;
bool startReset = false;
unsigned long resetTimer = 0;

//--------------

bool runningStatus = false;
bool readyStatus = false;
bool startingStatus = false;
bool starter1Status = false;
bool starter2Status = false;
bool startingErrorStatus = false;
bool engineStoppingStatus = false;

unsigned int engineErrorStatus = 0;

bool firstStart = true;

unsigned long timerSendDataToControler = 0;
unsigned long rpmTimer = 0;
unsigned long rpmNumberOfTeeth = 100;
unsigned int rpmValue = 0;
unsigned long rpmCounter = 0;
unsigned long heatingTimer = 0;
bool heatingFlag = false;


//serial event 2
void serialEvent2();
void checkIncomingData(char* data);
bool checkSum(char* data);
void SetNweValue(char* data);
void getValue(char* data);
void checkPanelKeyStatus(char* data);
void showKeyStatus();
void sendAllSettingsToPanel();
void setSerialNumber(char* serialNumber);
void getSerialNumber(char* serialNumber);
void getVersionSoftware(char* versionSoftware);
void getEepromName(char* buffer, int id);


//send data
void sendDataToPanel();
void makeStringToPanel(char* data);
void getTime(char* data);
void addRpm(char* data);
void addIntValueToString(char* data, int value);
void makeAnalogStringToPanel(char* data);
void makeCheckSum(char* data);

//sensor state
void getStnsorState();
void checkReset();
void setBit(unsigned int& value, int position);
void unSetBit(unsigned int& value, int position);
void setPinPositionMultiplexer(int position);


void SendToSd(uint8_t position);
void SendToSd(const char* data);

void rpmCount() {
	rpmCounter++;
	//	Serial.println("htutja ");
}

void setup() {
	Serial.begin(19200);
	Serial2.begin(19200);
	time.init();
	delay(500);
	SD.begin(53);
	setDefaultValues();	//read names from eeprom	
	Serial.println("Starting");
	attachInterrupt(digitalPinToInterrupt(2), rpmCount, RISING);
	Serial2.println("daialam");
	//set all digial pins
	for (size_t i = 0; i < 4; i++)
	{
		pinMode(muxPinPosition[i], OUTPUT);
		digitalWrite(muxPinPosition[i], LOW);
	}
	//set digital pins output
	for (size_t i = 0; i < sizeof(digitalOutArray) / sizeof(digitalOutArray[0]); i++)
	{
		pinMode(digitalOutArray[i], OUTPUT);
		digitalWrite(digitalOutArray[i], LOW);
	}
	//get all values from eeprom digital
	for (size_t i = 0; i < sizeof(digitalSensorArray) / sizeof(digitalSensorArray[0]); i++)
	{
		digitalSensorArray[i].getValuesFromEeprom();
	}
	//get all values from eeprom analog
	for (size_t i = 0; i < sizeof(analogSensorArray) / sizeof(analogSensorArray[0]); i++)
	{
		analogSensorArray[i].getValuesFromEeprom();
	}
	//---------------?????

	for (size_t i = 0; i < sizeof(digitalSensorArrayOutput) / sizeof(digitalSensorArrayOutput[0]); i++)
	{
		digitalSensorArrayOutput[i].getValuesFromEeprom();
	}
	setEngineStatus(ENGINE_READY, true);




}
void readAnalog(int id) {
	char buf[20];
	char unit[5];

	int adr = EEPROM_START_ANALOG + (id * 30);
	Global.getFromEeprom(adr, buf);
	Serial.println(adr);

	adr += 15; Serial.println(adr);
	Global.getFromEeprom(adr, unit);
	adr += 5; Serial.println(adr);
	int min = EEPROM.read(adr);

	adr += 3; Serial.println(adr);
	int max = EEPROM.read(adr);
	adr += 3; Serial.println(adr);
	int range = EEPROM.read(adr);

	char buffer[100];
	sprintf(buffer, "id = %d, name =  %s, unit =  %s, max = %d, min = %d, range = %d% ", id, buf, unit, max, min, range);
	Serial.println(buffer);
}
void getEepromName(char* buffer, int id) {
	//%
	int adr = EEPROM_START_ENGINE_STATE + (id * EEPROM_DIGITAL_SIZE);
	char buf[20];
	Global.getFromEeprom(adr, buf);
	sprintf(buffer, "%%/%d/%s/%s/*", id, buf, "0");

}
void getEepromName(int start, char* buffer, int id) {
	//%
	int adr = start + (id * EEPROM_DIGITAL_SIZE);
	char buf[20];
	Global.getFromEeprom(adr, buf);
	sprintf(buffer, "%%/%d/%s/%s/*", id, buf, "0");

}
void getEepromName(char startSign, int start, char* buffer, int id) {
	//%
	int adr = start + (id * EEPROM_DIGITAL_SIZE);
	char buf[20];
	Global.getFromEeprom(adr, buf);
	Serial.println(buf);
	sprintf(buffer, "%c/%d/%s/%s/*", startSign, id, buf, "0");

}

void setEepromName(int start, const char* buffer, int id) {

	int adr = start + (id * EEPROM_DIGITAL_SIZE);
	Global.saveToEprom(adr, buffer);


}


bool recived = false;
void loop() {


	getEngineSensorState();
	if (millis() - timer > 3000) { // check all data 

		timer = millis();


		//setBatteryState();
		//blink();
	}

	if (millis() - resetTimer > 5000 && startReset) {//??
		startReset = false;
		startingErrorStatus = false;

	}
	if (!runningStatus && !startingErrorStatus && !startReset && !keyOffState) {
		setEngineStatus(ENGINE_READY, true);
		setEngineStatus(CONTROLER_OUT, false);

	}
	if (keyOffState) {
		setEngineStatus(ENGINE_READY, false);
		setEngineStatus(CONTROLER_OUT, true);
		setEngineStatus(ENGINE_STARTING, false);
		setEngineStatus(ENGINE_ERROR, false);


	}
	if (keyAutoState && !startReset) {//na razie ie wiem dlaczego tak zrobiłem zastanów sie co to ma znaczyć 
		if (getWaterPressState() && !runningStatus && !startingErrorStatus) {
			starting();
		}
		else {
			//setEngineStatus(ENGINE_ERROR, true);
			setEngineStatus(CONTROLER_OUT, false);
		}


	}
	else if (keyAutoState && startReset) {
		setEngineStatus(ENGINE_READY, true);
	}

	else if (keyHandState)
	{
		if (startingErrorStatus) {
			setEngineStatus(ENGINE_ERROR, true);
			setEngineStatus(CONTROLER_OUT, false);
		}
		else
		{
			setEngineStatus(ENGINE_READY, true);
			setEngineStatus(CONTROLER_OUT, false);
		}


		if (!runningStatus && !startingErrorStatus && !startReset)
		{
			while (keyStart)
			{
				getEngineSensorState();
				handStart();
				if (!flagHandStarting) {
					flagHandStarting = true;
					setEngineStatus(ENGINE_READY, false);
					setEngineStatus(ENGINE_STARTING, true);
				}

				if (getRpm())
				{
					runningStatus = true;
					setEngineStatus(ENGINE_RUNNING, true);
					break;
				}
				if (startingErrorStatus) {

					break;
				}
			}
			if (flagHandStarting) //bylo odpalone recznie
			{
				setStartersState(LOW);
				setEngineStatus(ENGINE_STARTING, false);
				if (flagBattery)			//następnym razem zostanie wybrana inna bateria do rozruchu
					flagBattery = false;
				else
					flagBattery = true;
				flagHandStarting = false;
			}
		}
		else {
			setStartersState(LOW);

		}
	}
	if (runningStatus) {
		runningMode();
	}
}
void setEngineStatus(uint8_t position, bool state) {

	if (!state) {
		if (position < 5)
			engineErrorStatus &= ~(1 << position);
	}
	else {

		engineErrorStatus |= (1 << position);
	}
}
void runningMode() {
	static bool state = false;
	static bool readyToStop = false;
	unsigned long timer = 0;
	setEngineStatus(ENGINE_RUNNING, true);

	while (runningStatus) {
		getEngineSensorState();

		if ((keyAutoState && stopSignal() && !getWaterPressState() && !engineStoppingStatus) || (keyHandState && stopSignal() && !engineStoppingStatus)) {
			state = HIGH;
			sendStopSignal();
			engineStoppingStatus = true;

		}
		if (!getRpm() && engineStoppingStatus && !readyToStop) {
			timer = millis();
			readyToStop = true;
		}

		if (millis() - timer > 3000 && readyToStop) {
			engineStopped();
			state = LOW;
			readyToStop = false;
		}
	}

}

void setErrorState(bool state) {
	digitalWrite(digitalOutArray[ERROR], state);
	setEngineStatus(ENGINE_ERROR, state);
}

void engineStopped() {
	runningStatus = false;
	engineStoppingStatus = false;
	digitalWrite(digitalOutArray[STOP], LOW);
	setEngineStatus(ENGINE_RUNNING, false);
	setEngineStatus(ENGINE_READY, true);
	resetTimer = millis();
	startReset = true;
}
void sendStopSignal() {
	digitalWrite(digitalOutArray[STOP], HIGH);  //send stop signal
}

bool stopSignal() {
	return keyStop;
}
void getEngineSensorState() {
	serialEvent2();
	getStnsorState();
	checkReset();
	setBatteryState();
	setAnalogState();
	if (millis() - heatingTimer > 3000 && heatingFlag) {
		heatingFlag = false;
		digitalWrite(digitalOutArray[HEATING], LOW);
	}

	if (millis() - timerSendDataToControler > SEND_DATA)
	{
		sendDataToPanel();
		timerSendDataToControler = millis();
	}

	if (millis() - rpmTimer > 1000) {
		long temp = rpmCounter;
		rpmValue = (temp / rpmNumberOfTeeth) * 60;
		rpmCounter = 0;
		rpmTimer = millis();
	}
}

void setStartersState(bool state) {
	digitalWrite(digitalOutArray[STARTER_1], state);
	digitalWrite(digitalOutArray[STARTER_2], state);
}
void handStart() {
	if (flagBattery) {
		if (getBatterystate(BATTERY_1)) {//true start 1			
			digitalWrite(digitalOutArray[STARTER_1], HIGH);
		}
		else if (getBatterystate(BATTERY_2)) {
			flagBattery = false;
		}
		else
			handStartError();
	}
	else {
		if (getBatterystate(BATTERY_2)) {//true start 1			
			digitalWrite(digitalOutArray[STARTER_2], HIGH);
		}
		else if (getBatterystate(BATTERY_1)) {
			flagBattery = true;
		}
		else
			handStartError();
	}
}

void handStartError() {
	Serial.println("error hand start");
	digitalWrite(digitalOutArray[ERROR], HIGH);
	setEngineStatus(ENGINE_ERROR, true);
	startingErrorStatus = true;
	startReset = true;
	resetTimer = millis();
}
void starting() {
	setEngineStatus(ENGINE_READY, false);
	setEngineStatus(ENGINE_STARTING, true);
	int start = 0;
	int startTime = 2000;
	unsigned long startingTimer = millis();
	bool pause = false;
	getBatterystate(BATTERY_1);
	getRpm();
	while (start < 6) {
		getEngineSensorState();
		if (getRpm()) {
			engineStarted();
			break;
		}
		if (keyOffState) {
			digitalWrite(digitalOutArray[STARTER_1], LOW);
			digitalWrite(digitalOutArray[STARTER_2], LOW);
			setEngineStatus(ENGINE_STARTING, false);
			setEngineStatus(CONTROLER_OUT, true);
			break;
		}

		if (pause) {
			if (millis() - startingTimer > startTime) {
				pause = false;
				startingTimer = millis();
			}

		}
		else {
			getRpm();
			if (flagBattery) {
				if (getBatterystate(BATTERY_1)) {//true start 1
					digitalWrite(digitalOutArray[STARTER_1], HIGH);
					if (millis() - startingTimer > startTime) {
						Serial.println("start batteri 1");
						start++;
						flagBattery = false;
						digitalWrite(digitalOutArray[STARTER_1], LOW);
						startingTimer = millis();
						pause = true;
					}

				}
				else if (getBatterystate(BATTERY_2)) {
					flagBattery = false;
				}
				else
					start = 6;
			}
			else {
				if (getBatterystate(BATTERY_2)) {//true start 1
					digitalWrite(digitalOutArray[STARTER_2], HIGH);
					if (millis() - startingTimer > startTime) {
						Serial.println("start batteri 2");
						start++;
						flagBattery = true;
						digitalWrite(digitalOutArray[STARTER_2], LOW);
						startingTimer = millis();
						pause = true;
					}
				}
				else if (getBatterystate(BATTERY_1)) {
					flagBattery = true;
				}
				else
					start = 6;
			}
		}

	}
	if (start == 6) {
		startingErrorStatus = true;
		digitalWrite(digitalOutArray[ERROR], HIGH);
		setEngineStatus(ENGINE_STARTING, false);
		setEngineStatus(ENGINE_ERROR, true);
		digitalWrite(digitalOutArray[STARTER_1], LOW);
		digitalWrite(digitalOutArray[STARTER_2], LOW);
	}
}

void engineStarted() {
	setEngineStatus(ENGINE_STARTING, false);
	setEngineStatus(ENGINE_RUNNING, true);
	runningStatus = true;
	digitalWrite(digitalOutArray[STARTER_1], LOW);
	digitalWrite(digitalOutArray[STARTER_2], LOW);
}

int getRpm() {
	if (engineStoppingStatus)
		return rpmValue;
	if (rpmValue > 600)
		return true;
	else
		return false;
}

void setAnalogState() {
	setEngineStatus(BATTERY1_FAIL, analogSensorArray[0].getState());
	setEngineStatus(BATTERY2_FAIL, analogSensorArray[1].getState());
	setEngineStatus(ANALOG3_FAIL, analogSensorArray[2].getState());
	setEngineStatus(ANALOG4_FAIL, analogSensorArray[3].getState());
}

void setBatteryState() {
	setEngineStatus(BATTERY1_FAIL, analogSensorArray[0].getState());
	setEngineStatus(BATTERY2_FAIL, analogSensorArray[1].getState());

	if (analogSensorArray[0].getState() && analogSensorArray[1].getState()) {
		setEngineStatus(ENGINE_READY, false);
		setEngineStatus(ENGINE_ERROR, true);
	}

}

bool getBatterystate(int8_t battery) {
	if (analogSensorArray[battery].getState())
		return false;
	else
		return true;
	return false;
}

bool handStartingProces() {
	return flagHandStarting;// digitalRead(outputArray[STARTING]);
}

bool getWaterPressState() {
	return  digitalSensorArray[SYSTEM_PRESURE].getState();
}

void blink() {
	static bool a = false;

	if (a)
		a = false;
	else
		a = true;
	for (size_t i = 0; i < sizeof(digitalOutArray) / sizeof(digitalOutArray[0]); i++)
		digitalWrite(digitalOutArray[i], a);
}
void defaultValues() {
	Serial.println("tutaj");
	EEPROM.write(0, 1);
	setDefaultValues();
	for (int i = 0; i < sizeof(digitalSensorArray) / sizeof(digitalSensorArray[0]); i++)
	{
		digitalSensorArray[i].getValuesFromEeprom();
	}
	for (size_t i = 0; i < sizeof(analogSensorArray) / sizeof(analogSensorArray[0]); i++)
	{
		analogSensorArray[i].getValuesFromEeprom();
	}
	sendAllSettingsToPanel();
}
void setDefaultValues() {
	if (EEPROM.read(0) == 0) {
		Serial.println("nic do roboty");
		return;
	}
	int start = 0;
	EEPROM.write(0, 0);  //start
	EEPROM.write(1, 5);  //delay
	EEPROM.write(2, 6);  //number of attempts to start
	EEPROM.write(EEPROM_ADR_AMOUNT_OF_THEET, EEPROM_AMOUNT_OF_THEET);//amount of teeth	
	Global.saveToEprom(EEPROM_ADR_SERIAL_NUMBER, EEPROM_SERIAL_NUMBER);//serial number adr 20 -39	
	Global.saveToEprom(EEPROM_ADR_SOFTWARE_VERSION, softwareVersion);//software version 40 - 49

	const char* namesIn[] = { "Pressure","Oil pressure",
		"Temp. low","Temp. high", "Flow water", "Fuel level low","Fuel valve", "Fueltank leak",
		"Charger 1 fail", "Charger 2 fail", "Starter 1 fail", "Starter 1 fail", "User 1","User 2","User 3","User 4"
	};

	start = 200;
	for (size_t i = 0; i < 16; i++)
	{
		int adr = start + (i * EEPROM_DIGITAL_SIZE);
		Global.saveToEprom(adr, namesIn[i]);
		adr += 15;
		EEPROM.write(adr, 0); //Serial.print("state : "); Serial.println(adr);
	}

	const char* namesOut[] = { "Starter 1" , "Starter 2" , "Stop", "Error", "Heating" };
	start = 520;
	for (size_t i = 0; i < 16; i++)
	{
		int adr = start + (i * EEPROM_DIGITAL_SIZE);
		if (i < 5)
			Global.saveToEprom(adr, namesOut[i]);
		else

			Global.saveToEprom(adr, "User out"); //Serial.print("name : "); Serial.println(adr);
		adr += 15;
		EEPROM.write(adr, 0); //Serial.print("state : "); Serial.println(adr);
	}

	const char* engineState[] = { "Controler out" , "Ready" , "Starting", "Running", "Starting error", "Battery1 fail", "Battery2 fail","Analog 3", "Analog 4" };
	start = EEPROM_START_ENGINE_STATE;
	for (size_t i = 0; i < 16; i++)
	{
		int adr = start + (i * EEPROM_DIGITAL_SIZE);
		if (i < sizeof(engineState) / sizeof(engineState[0]))
			Global.saveToEprom(adr, engineState[i]);
		else

			Global.saveToEprom(adr, "Engine State"); //Serial.print("name : "); Serial.println(adr);
		adr += 15;
		EEPROM.write(adr, 0); //Serial.print("state : "); Serial.println(adr);
	}

	start = EEPROM_START_ANALOG;

	for (int i = 0; i < 16; i++)
	{
		int adr = start + (i * EEPROM_ANALOG_SIZE); //Serial.print("name : "); Serial.println(adr);
		if (i == 0)
			Global.saveToEprom(adr, "Battery 1");// name

		else if (i == 1)
			Global.saveToEprom(adr, "Battery 2");// name
		else
			Global.saveToEprom(adr, "AnalogValue");// name		
		adr += 15; //Serial.print("unit : "); Serial.println(adr);
		Global.saveToEprom(adr, "V");//unit
		adr += 5; //Serial.print("min : "); Serial.println(adr);
		EEPROM.write(adr, 2);//min
		adr += 3; //Serial.print("max : "); Serial.println(adr);
		EEPROM.write(adr, 4);//max
		adr += 3; //Serial.print("range : "); Serial.println(adr);
		EEPROM.write(adr, 5);//range


	}
}

