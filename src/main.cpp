/**
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

RzWifi myWifi(DNS_NAME, WIFI_CREDENTIALS, 3);
RzTime myTime;
RzFiles myFiles;
RzTemperature temp(&myTime, ONE_WIRE_PIN, 60 * 1000);
RzServer myServer(80, &myTime, &myFiles, &temp);

void setup() {
    Serial.begin(115200);
    //Serial.setDebugOutput(true);
    delay(100);

    uint16_t uChipId = ESP.getChipId();
    Serial.printf("Booting Chip: %X\r\n", uChipId);

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED off (HIGH is the voltage level)

    myWifi.setup();     // initilize wifi
    myTime.setup();     // synchronize time

    myFiles.setup();    // Start the SPI Flash Files System
    temp.setup();       // initialize the thermometer


    myServer.setup();   // HTTP server

    uint32_t hfree;
    uint16_t hmax;
    uint8_t hfrag;
    ESP.getHeapStats(&hfree, &hmax, &hfrag);
    Serial.printf("%d Free, %d max, %d Fragment.", hfree, hmax, hfrag);

    Serial.println();
    delay(1000);    // just a bit of time to let asynchronous task to finish
    Serial.println("Node is started.");
}

// the loop function runs over and over again forever
void loop() {
    unsigned long currentMillis = millis();
    myWifi.loop(currentMillis);
    myFiles.loop(currentMillis);
    myServer.loop(currentMillis);
    temp.loop(currentMillis);
}
