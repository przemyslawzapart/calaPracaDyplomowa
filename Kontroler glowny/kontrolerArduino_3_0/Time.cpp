
#include <DS1307.h>
#include <Wire.h>

#include "Time.h"


DS1307 clock;
RTCDateTime dt;

void getData(const char* data, char* buf, int start, int len) {  //tested ok
    char bufTemp[40];                   //temp buffer
    for (int i = 0; i < len; i++) {      //send data to buffer
        bufTemp[i] = data[start + i];
    }
    bufTemp[len] = 0;//terminate byte
    strcpy(buf, bufTemp);
}

void TimeClass::init() {
    clock.begin();
    clock.setDateTime(__DATE__, __TIME__);
}

void TimeClass::hexTime(char* buffer) {
    char temp[10];
    getNewTime();
    sprintf(temp, "%02x", dt.year - 2000);
    strcpy(buffer, temp);
    sprintf(temp, "%02x", dt.month);
    strcat(buffer, temp);
    sprintf(temp, "%02x", dt.day);
    strcat(buffer, temp);
    sprintf(temp, "%02x", dt.hour);
    strcat(buffer, temp);
    sprintf(temp, "%02x", dt.minute);
    strcat(buffer, temp);
    sprintf(temp, "%02x", dt.second);
    strcat(buffer, temp);

}
void TimeClass::getTimeStamp(char* buffer) {
    dt = clock.getDateTime();
    sprintf(buffer, "%02d-%02d-%02d %02d:%02d:%02d/", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
}
void TimeClass::timeStamp(char* buffer) {
    getNewTime();
    char buf[25];
    strcpy(buf, yearSaved);
    strcat(buf, "/");
    strcat(buf, monthSaved);
    strcat(buf, "/");
    strcat(buf, daySaved);
    strcat(buf, ";");
    strcat(buf, hourSaved);
    strcat(buf, ":");
    strcat(buf, minuteSaved);
    strcat(buf, ":");
    strcat(buf, secondSaved);
    strcat(buf, ";");
    strcpy(buffer, buf);
}
void TimeClass::getNewTime() {
    dt = clock.getDateTime();
    sprintf(yearSaved, "%02d", dt.year - 2000);
    sprintf(monthSaved, "%02d", dt.month);
    sprintf(daySaved, "%02d", dt.day);
    sprintf(hourSaved, "%02d", dt.hour);
    sprintf(minuteSaved, "%02d", dt.minute);
    sprintf(secondSaved, "%02d", dt.second);
}
void TimeClass::setNewTime(char* newTime) {
    char tempBuf[20];

    getData(newTime, tempBuf, 0, 2);
    int y = atoi(tempBuf);
    getData(newTime, tempBuf, 2, 2);
    int m = atoi(tempBuf);
    getData(newTime, tempBuf, 4, 2);
    int d = atoi(tempBuf);
    getData(newTime, tempBuf, 6, 2);
    int h = atoi(tempBuf);
    getData(newTime, tempBuf, 8, 2);
    int i = atoi(tempBuf);
    getData(newTime, tempBuf, 10, 2);
    int s = atoi(tempBuf);
    clock.setDateTime(y + 2000, m, d, h, i, s);

}

void TimeClass::getYear(char* buffer) {
    dt = clock.getDateTime();
    sprintf(buffer, "%02d", (dt.year - 2000));
}


TimeClass Time;
