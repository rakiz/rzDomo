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
#include "network/RzServer.h"
#include "sensors/RzTemperature.h"
#include "sensors/RzMemory.h"

// Data wire is plugged into GPIO4 (D2) on the ESP8266
#define ONE_WIRE_PIN 4

RzWifi *wifi;
RzComponents components;

void setup() {
    Serial.begin(115200);
    //Serial.setDebugOutput(true);
    delay(100);

    uint16_t uChipId = ESP.getChipId();
    Serial.printf("Booting Chip: %X\r\n", uChipId);

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED off (HIGH is the voltage level)

    // Initialize the Wifi & synchronized time
    wifi = new RzWifi();
    components.add(wifi);

    // Initialize the SPI Flash Files System
    components.add(new RzFiles());

    // Initialize the Thermometer
    //components.add(new RzTemperature(ONE_WIRE_PIN, "tp", "Temperature Piscine"));

    // Initialize Memory monitoring
    components.add(new RzMemory());

    // Initialize the Web server
    components.add(new RzServer(80, components));

//    components.setup();

    Serial.println();
    Serial.println("Node is started and ready to rumble.");
    Serial.println();
}

// the loop function runs over and over again forever
void loop() {
    timeMs now = wifi->getTime();
    while(wifi->isTimeSynchronized() && now<MIN_TIMESTAMP) { // wait for time synchro
        delay(1000);
        now = wifi->getTime();
    }
    components.loop(now);
}
