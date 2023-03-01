
void getStnsorState() {
	static bool engineStopped = false;

	if (runningStatus)
		engineStopped = true;
	checkReset();
	for (size_t i = 0; i < sizeof(digitalSensorArray) / sizeof(digitalSensorArray[0]); i++)
	{
		if (i == ENGINE_TEMPERATURE_LOW) {
			if (digitalSensorArray[ENGINE_TEMPERATURE_LOW].getState()) {
				digitalWrite(digitalOutArray[HEATING], HIGH);
				heatingFlag = true;
				heatingTimer = millis();
			}
		}
		if (!runningStatus && (i == OIL_PRESURE || i == ENGINE_TEMPERATURE_HIGH || i == FLOW_RAW_WATER)) {
			if (engineStopped) {
				digitalInputState &= ~(1 << OIL_PRESURE);
				digitalInputState &= ~(1 << ENGINE_TEMPERATURE_HIGH);
				digitalInputState &= ~(1 << FLOW_RAW_WATER);
				engineStopped = false;
			}
			continue;
		}
		bool currentState = digitalSensorArray[i].getValue();
		bool previousState = digitalInputState & (1 << i);
		if (currentState != previousState) {
			if (currentState) {
				if (!firstStart) {
					SendToSd(i);
				}

				digitalInputState |= (1 << i);
				//ustaw errors
				if (i != ENGINE_TEMPERATURE_LOW)
					digitalWrite(digitalOutArray[ERROR], HIGH);
			}
			else {
				digitalInputState &= ~(1 << i);
				if (i != ENGINE_TEMPERATURE_LOW)
					digitalWrite(digitalOutArray[ERROR], LOW);
			}
		}
	}
	for (size_t i = 0; i < sizeof(digitalOutArray) / sizeof(digitalOutArray[0]); i++)
	{
		if (digitalRead(digitalOutArray[i]))
			digitalOutputState |= (1 << i);
		else
			digitalOutputState &= ~(1 << i);
	}

	for (size_t i = 0; i < sizeof(analogSensorArray) / sizeof(analogSensorArray[0]); i++)
	{
		setPinPositionMultiplexer(i);
		analogSensorArray[i].measurement();
	}

	if (firstStart) {
		firstStart = false;
	}
}
void read() {
	int digitalOut[] = { 8,9,10,11,12,13,22,23,24,25,26,27,28,29,30,31 };
	int digitalIn[] = { 32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47 };

	Serial.print("digital in : ");
	for (size_t i = 0; i < sizeof(digitalIn) / sizeof(digitalIn[0]); i++)
	{
		Serial.print(digitalRead(digitalIn[i]));
	}
	Serial.println();

	Serial.print("digital out: ");
	for (size_t i = 0; i < sizeof(digitalOut) / sizeof(digitalOut[0]); i++)
	{
		Serial.print(digitalRead(digitalOut[i]));
	}
	Serial.println();
}
void checkReset() {
	if (keyReset) {
		if (!resetFlag) {
			SendToSd("Reset");
			resetFlag = true;
			resetState = false;
			setErrorState(false);

			if (startingErrorStatus) {
				startReset = true;
				resetTimer = millis();
			}

			for (size_t i = 0; i < sizeof(digitalSensorArray) / sizeof(digitalSensorArray[i]); i++)
			{
				digitalSensorArray[i].resetSensor();
			}
			startingErrorStatus = false;
			for (size_t i = 0; i < 4; i++)//zaczynajac od baterii
			{
				engineErrorStatus &= ~(1 << i + 5);
			}
		}
	}
	else {
		resetFlag = false;
	}

}

void setBit(unsigned int& value, int position) {
	value |= (1 << position);
}
void unSetBit(unsigned int& value, int position) {
	value &= ~(1 << position);
}
void setPinPositionMultiplexer(int position) {
	for (size_t i = 0; i < sizeof(muxPinPosition) / sizeof(muxPinPosition[0]); i++)
	{
		bool state = false;
		if (position & (1 << i))
			state = true;
		digitalWrite(muxPinPosition[i], state);
	}

}
