/**
 * https://circuits4you.com/2016/12/16/esp8266-serial-communication/
 * https://favicon.io/
 * https://diyprojects.io/esp8266-web-server-part-4-arduinojson-load-save-files-spiffs/
 * https://github.com/me-no-dev/arduino-esp32fs-plugin
 * https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/examples/FSBrowser/FSBrowser.ino
 * https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
 * https://github.com/HarringayMakerSpace/ESPDailyTask
 * https://randomnerdtutorials.com/esp8266-daily-task-publish-temperature-readings-to-thingspeak/
 * https://randomnerdtutorials.com/esp8266-publishing-dht22-readings-with-mqtt-to-raspberry-pi/
 * https://iotdesignpro.com/projects/iot-based-water-flow-meter
 * https://iotdesignpro.com/projects/iot-based-air-quality-monitoring-system-with-twitter-notification
 * https://iotdesignpro.com/projects/iot-based-food-monitoring-system
 * https://scidle.com/how-to-use-a-ph-sensor-with-arduino/
 * https://www.geekstips.com/mq4-sensor-natural-gas-methane-arduino/
 * https://github.com/stevemarple/IniFile
 * https://www.compuphase.com/minini.htm
 * https://github.com/compuphase/minIni
 * https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266mDNS/examples/OTA-mDNS-LittleFS/OTA-mDNS-SPIFFS.ino
 * https://www.megunolink.com/documentation/connecting/mdns-browser/ <- could be used to conecct ESP together (mdns is not working from browser)
 * String error = String();
 * error += F("!NO_LEADING_SLASH! ");
 */
#include <Arduino.h>

#include "RzFiles.h"
#include "network/RzWifi.h"
#include "network/RzTime.h"
#include "network/RzServer.h"
#include "sensors/RzTemperature.h"

// Data wire is plugged into GPIO4 (D2) on the ESP8266
#define ONE_WIRE_PIN 4

String DNS_NAME = "piscine";
String WIFI_CREDENTIALS[] = {
        "BelaTegeuse_5.0", "ledormeurdoitsereveiller",
        "BelaTegeuse_2.4", "ledormeurdoitsereveiller",
        "Salusa-Secondus", "ledormeurdoitsereveiller"
};

std::vector<RzLoop *> rzLoops;

void setup() {
    Serial.begin(115200);
    //Serial.setDebugOutput(true);
    delay(100);

    uint16_t uChipId = ESP.getChipId();
    Serial.printf("Booting Chip: %X\r\n", uChipId);

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED off (HIGH is the voltage level)

    // Initialize the Wifi
    RzWifi *myWifi = new RzWifi(DNS_NAME, WIFI_CREDENTIALS, 3);
    rzLoops.push_back(myWifi);
    myWifi->setup();

    // Initialize the Time Synchronization (should be merged with wifi)
    RzTime *myTime = new RzTime();
    rzLoops.push_back(myTime);
    myTime->setup();

    // Initialize the SPI Flash Files System
    RzFiles *myFiles = new RzFiles();
    rzLoops.push_back(myFiles);
    myFiles->setup();    // Start the SPI Flash Files System

    // Initialize the Thermometer
    RzTemperature *temp = new RzTemperature(myTime, ONE_WIRE_PIN, 60 * 1000);
    rzLoops.push_back(temp);
    temp->setup();

    // Initialize the Web server
    RzServer *myServer = new RzServer(80, myTime, myFiles, temp);
    rzLoops.push_back(myServer);
    myServer->setup();

    uint32_t hfree;
    uint16_t hmax;
    uint8_t hfrag;
    ESP.getHeapStats(&hfree, &hmax, &hfrag);
    Serial.printf("%d Free, %d max, %d Fragment.\r\n", hfree, hmax, hfrag);

    Serial.println();
    Serial.println("Node is started and ready to rumble.");
    Serial.println();
}

// the loop function runs over and over again forever
void loop() {
    unsigned long currentMillis = millis();
    for (unsigned int i(0); i < rzLoops.size(); ++i) {
        rzLoops[i]->loop(currentMillis);
    }
}
