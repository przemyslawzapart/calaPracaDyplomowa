
void sendDataToPanel() {
	char dataToSend[300];
	makeStringToPanel(dataToSend);
	Serial.println(dataToSend);
	Serial2.println(dataToSend);
}
void makeStringToPanel(char* data) {
	char time[25];
	strcpy(data, "#");
	getTime(time);
	strcat(data, time);
	addRpm(data);//rpm
	addIntValueToString(data, engineErrorStatus);//???
	addIntValueToString(data, digitalInputState);
	addIntValueToString(data, digitalOutputState);
	makeAnalogStringToPanel(data);
	makeCheckSum(data);
}
void getTime(char* data) {
	time.getTimeStamp(data);
}
void addRpm(char* data) {
	char buf[5];
	sprintf(buf, "%d/", rpmValue);
	strcat(data, buf);
}
void addIntValueToString(char* data, int value) {
	char buf[5];
	sprintf(buf, "%04x", value);
	strcat(data, buf);
	strcat(data, "/");
}
void makeAnalogStringToPanel(char* data) {
	for (size_t i = 0; i < sizeof(analogSensorArray) / sizeof(analogSensorArray[0]); i++)
	{
		char buffer[10];
		int value = analogSensorArray[i].getValue();
		int first = value / 10;
		int second = value - (first * 10);
		sprintf(buffer, "%03x", value);
		strcat(data, buffer);
		strcat(data, "/");
	}
}
void makeCheckSum(char* data) {
	int len = strlen(data);
	len += 3;
	char buf[3];
	sprintf(buf, "%02x", len);
	strcat(data, buf);
	strcat(data, "*");
}
