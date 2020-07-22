#ifndef _MY_TIME_H_
#define _MY_TIME_H_

#include <NTPClient.h>
#include <WiFiUdp.h>

class RzTime {
public:
    RzTime();

    void setup();

    void loop(unsigned long _currentMillis);

    unsigned long getEpochTime();

    String formatDate(unsigned long epochTime);

private:
    //Week Days
    const String weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    //Month names
    const String months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September",
                               "October", "November", "December"};

    WiFiUDP *ntpUDP;
    NTPClient *timeClient;
};

#endif
