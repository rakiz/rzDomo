#ifndef _RZ_WIFI_H_
#define _RZ_WIFI_H_

#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "base/RzComponent.h"
#include "base/RzConfigurable.h"

#define MIN_TIMESTAMP 1577836800

class RzWifi : public RzComponent {
public:
    explicit RzWifi();

    virtual ~RzWifi();

    bool __unused isConnected();

    void setup() override;

    void loop(timeMs referenceTime) override;

    void loadConfiguration() override;

    void saveConfiguration() override;

    const char *getId() override;

    const char *getDisplayName() override;

    const char *getJsonConfig() override;

    const char *getPrefix() override;

    // ---------------------

    ulong getTime();

    static String formatDate(timeMs time);

    inline bool isTimeSynchronized() const { return _isTimeSynchronized; }


private:
    //Week Days
    //const char *_weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    //Month names
    //const char *_months[12] = {"January", "February", "March", "April", "May", "June",
    //                           "July", "August", "September", "October", "November", "December"};

    bool _isTimeSynchronized = false;

    ESP8266WiFiMulti *_wifiMulti;
    WiFiUDP *_ntpUDP;
    NTPClient *_timeClient;
    // parameters

    const char *_dnsName = "test";
    // WifiAPEntry
    const char *_ssid = "BelaTegeuse_2.4";
    const char *_password = "ledormeurdoitsereveiller";

    const char *_poolServerName = "time.nist.gov";
    uint _timeOffset = 0;
    uint _updateInterval = 60000;  // In ms

    static void initSoftAP();

};

#endif