void serialEvent2() {

	static String data = "";
	while (Serial2.available()) {
		char c = (char)Serial2.read();
		data += c;
		if (c == '*') {
			data.trim();
			const char* dataChar = data.c_str();
			checkIncomingData((char*)dataChar);
			data = "";
		}
	}
}

void checkIncomingData(char* data) {
	char c = data[0];
	if (c == '#') {
		SetNweValue(data);
	}
	else if (c == '@') {
		getValue(data);
	}
	else if (c == '!') {
		checkPanelKeyStatus(data);
	}
}

bool checkSum(char* data) {
	return true;
	int len = strlen(data);
	char buf[3];
	Global.getData(data, buf, len - 3, 2);
	uint8_t checkSum = strtol(buf, NULL, 16);
	if (len == checkSum)
		return true;
	else
		return false;
}
void SetNweValue(char* data) {
	char korektor[] = "#/*";
	char* schowek;
	schowek = strtok(data, korektor);

	if (strcmp(schowek, "S") == 0) {
		schowek = strtok(NULL, korektor);
		setSerialNumber(schowek);
		Serial.print("new srn = ");
		char b[20];
		getSerialNumber(b);
		Serial.println(b);
	}
	else if (strcmp(schowek, "A") == 0) {
		int id = atoi(strtok(NULL, korektor));
		char* name = strtok(NULL, korektor);//name
		char* unit = strtok(NULL, korektor);//unit
		int min = atoi(strtok(NULL, korektor));
		int max = atoi(strtok(NULL, korektor));
		int range = atoi(strtok(NULL, korektor));

		analogSensorArray[id].changeValues(name, unit, min, max, range);
		char buf[45];
		analogSensorArray[id].getSettingsValues(buf);
		Serial.print(buf);
		Serial2.println(buf);

		int engineStateId = id + 5;
		setEepromName(EEPROM_START_ENGINE_STATE, name, engineStateId);
		getEepromName('!', EEPROM_START_ENGINE_STATE, buf, engineStateId);
		Serial.println(buf);
		Serial2.println(buf);
	}
	else if (strcmp(schowek, "D") == 0) {
		int id = atoi(strtok(NULL, korektor));
		char* name = strtok(NULL, korektor);//name
		int contact = atoi(strtok(NULL, korektor));

		digitalSensorArray[id].changeValue(name, contact);
		char buf[45];
		digitalSensorArray[id].getSettingsValues(buf);
		Serial.println(buf);
		Serial2.print(buf);
	}
	else if (strcmp(schowek, "O") == 0) {
		int id = atoi(strtok(NULL, korektor));
		char* name = strtok(NULL, korektor);//name
		int contact = atoi(strtok(NULL, korektor));
		char buf[45];

		setEepromName(EEPROM_START_DIGITAL_OUTPUT, name, id);
		getEepromName('&', EEPROM_START_DIGITAL_OUTPUT, buf, id);
		Serial.println(buf);
		Serial2.println(buf);
	}
	else if (strcmp(schowek, "Q") == 0) {
		schowek = strtok(NULL, korektor);
		Serial.println("Default values");
		defaultValues();
	}
}
void getValue(char* data) {
	char korektor[] = "@/*";
	char* schowek;
	schowek = strtok(data, korektor);
	if (strcmp(schowek, "A") == 0) {
		int id = atoi(strtok(NULL, korektor));
		char buf[45];
		analogSensorArray[id].getSettingsValues(buf);
		Serial.println(buf);
		Serial2.println(buf);
	}
	else if (strcmp(schowek, "D") == 0) {
		int id = atoi(strtok(NULL, korektor));
		char buf[45];
		digitalSensorArray[id].getSettingsValues(buf);
		Serial.println(buf);
		Serial2.println(buf);
	}
	else if (strcmp(schowek, "O") == 0) {
		int id = atoi(strtok(NULL, korektor));
		char buf[45];
		digitalSensorArrayOutput[id].getSettingsValues(buf);
		Serial.println(buf);
		Serial2.println(buf);
	}
	else if (strcmp(schowek, "S") == 0) {
		char buf[12];
		getSerialNumber(buf);
		char bufferToSend[20];
		strcpy(bufferToSend, "%S/");
		strcat(bufferToSend, buf);
		strcat(bufferToSend, "*");
		Serial.println(bufferToSend);
		Serial2.println(bufferToSend);
	}

	else if (strcmp(schowek, "Q") == 0) { //send all data to panel
		sendAllSettingsToPanel();
	}
}
void checkPanelKeyStatus(char* data) {
	char korektor[] = "!*";
	char* buffer;
	buffer = strtok(data, korektor);
	unsigned long value = strtol(buffer, NULL, 16);
	bool temp = (value & (1 << 4));
	static long timerStart = 0;
	static bool start = false;
	if (keyStart != temp && !start) {
		start = true;
		timerStart = millis();
	}
	else if (keyStart == temp && start) {
		start = false;
	}
	if (millis() - timerStart > 50 && start && (keyStart != temp)) {
		start = false;
		keyStart = value & (1 << 4);
	}

	keyStop = value & (1 << 5);
	keyReset = value & (1 << 6);

	keyHandState = value & (1 << 0);
	keyOffState = value & (1 << 1);
	keyAutoState = value & (1 << 2);
}
void showKeyStatus() {
	Serial.print(keyAutoState);
	Serial.print(keyHandState);
	Serial.print(keyOffState);
	Serial.print(keyStart);
	Serial.print(keyStop);
	Serial.print(keyReset);
	Serial.println();
}

void sendAllSettingsToPanel() {
	//$id/name/contact*

	for (int i = 0; i < sizeof(digitalSensorArray) / sizeof(digitalSensorArray[0]); i++)
	{
		char data[100];
		digitalSensorArray[i].getSettingsValues(data);
		Serial.println(data);

		Serial2.println(data);
	}
	for (size_t i = 0; i < sizeof(analogSensorArray) / sizeof(analogSensorArray[0]); i++)
	{
		char data[100];
		analogSensorArray[i].getSettingsValues(data);
		Serial.println(data);
		Serial2.println(data);
	}
	for (size_t i = 0; i < 16; i++) {
		char data[100];
		getEepromName('!', EEPROM_START_ENGINE_STATE, data, i),
			Serial.println(data);
		Serial2.println(data);
	}
	for (size_t i = 0; i < sizeof(digitalOutArray) / sizeof(digitalOutArray[0]); i++) {
		char data[100];
		getEepromName('&', EEPROM_START_DIGITAL_OUTPUT, data, i),
			Serial.println(data);
		Serial2.println(data);
	}
	char data[100];
	char buf[20];
	strcpy(data, "%/S/");
	getSerialNumber(buf);
	strcat(data, buf);
	strcat(data, "*");
	Serial.println(data);
	Serial2.println(data);

	strcpy(data, "%/V/");
	getVersionSoftware(buf);
	strcat(data, buf);
	strcat(data, "*");
	Serial.println(data);
	Serial2.println(data);
}
void setSerialNumber(char* serialNumber) {
	Global.saveToEprom(EEPROM_ADR_SERIAL_NUMBER, serialNumber);

}
void getSerialNumber(char* serialNumber) {
	Global.getFromEeprom(EEPROM_ADR_SERIAL_NUMBER, serialNumber);
}
void getVersionSoftware(char* versionSoftware) {
	Global.getFromEeprom(EEPROM_ADR_SOFTWARE_VERSION, versionSoftware);
}
void getAmountOfTheet(char* amountOfTheet) {
	EEPROM.read(EEPROM_ADR_AMOUNT_OF_THEET);
}
