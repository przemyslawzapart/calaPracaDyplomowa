// 
// 
// 

#include "Global.h"
#include <EEPROM.h>

void GlobalClass::init(){}
void GlobalClass::getFromEeprom(int address, char* data) {
//	strcpy(data, "");
	int pos = 0;
	char c = (char)EEPROM.read(address++);
	while (c != 0) {
		data[pos++] = c;
		c = (char)EEPROM.read(address++);	
	}
	data[pos] = 0;
}
void GlobalClass::saveToEprom(int address, const char* data) {
	while (*data)
		EEPROM.write(address++, *data++);
//	EEPROM.write(address, '\n');
	EEPROM.write(address, 0);
}
bool GlobalClass::getData(const char* data, char* buffer, int start, int len) {	
	strcpy(buffer, "");
	int position = 0;
	while (len > 0) {
		buffer[position++] = data[start++];
		len--;
	}
	buffer[position] = '\n';
}



GlobalClass Global;

