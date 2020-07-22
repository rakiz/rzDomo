#include "RzTime.h"

RzTime::RzTime() {
    ntpUDP = new WiFiUDP();
    timeClient = new NTPClient(*ntpUDP);
}

void RzTime::setup() {
    timeClient->begin();
    //delay(100);
    // Sometimes the NTPClient retrieves the year of 1970. To ensure that doesn’t happen we force the update.
    while(!timeClient->update()) {
        timeClient->forceUpdate();
    }
    //delay(1000);
    //timeClient->setTimeOffset(2);
    Serial.printf("Time is synchronized: %s\r\n", formatDate(getEpochTime()).c_str());
}

void RzTime::loop(unsigned long _currentMillis) {
    timeClient->update();
}

unsigned long RzTime::getEpochTime() { return timeClient->getEpochTime(); }

String RzTime::formatDate(unsigned long epochTime) {
    struct tm *ptm = gmtime((time_t * ) & epochTime);
    char buffer[25];
    //int len=
    sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02dZ", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour,
            ptm->tm_min, ptm->tm_sec);
    return String(buffer);
}

// https://tttapa.github.io/ESP8266/Chap15%20-%20NTP.html