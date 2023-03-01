void SendToSd(uint8_t position) {
	File file = SD.open("log.csv", FILE_WRITE);
	if (!file) {
		Serial.println("SD ERROR");
		return;
	}

	char dataToSd[200];
	char buf[100];
	time.timeStamp(dataToSd);
	getAnalogString(buf);
	strcat(dataToSd, digitalSensorArray[position].getName());
	strcat(dataToSd, buf);
	file.println(dataToSd);
	Serial.println(dataToSd);
	file.close();
}
void SendToSd(const char* data) {
	File file = SD.open("log.csv", FILE_WRITE);
	if (!file)
		return;
	char dataToSd[200];
	time.timeStamp(dataToSd);
	strcat(dataToSd, data);
	file.println(dataToSd);
	Serial.println(dataToSd);
	file.close();
}
void getAnalogString(char* buffer) {
	strcpy(buffer, ";");
	char buf[10];
	for (int i = 0; i < sizeof(analogSensorArray) / sizeof(analogSensorArray[0]); i++) {
		analogSensorArray[i].getFlaotValue(buf);
		strcat(buffer, buf);
		strcat(buffer, ";");
	}
}

