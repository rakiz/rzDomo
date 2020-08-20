#include <Arduino.h>
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <cstdint>

#include "RzWifi.h"
#include "tools/Tools.h"

RzWifi::RzWifi() {
    RzWifi::loadConfiguration();
    _wifiMulti = new ESP8266WiFiMulti();
    _ntpUDP = new WiFiUDP();
    _timeClient = new NTPClient(*_ntpUDP);
}

RzWifi::~RzWifi() {
    delete _ntpUDP;
    delete _timeClient;
    delete _wifiMulti;
}

bool __unused RzWifi::isConnected() {
    if (strlen(_ssid) == 0 || strlen(_password) == 0) {
        return false;
    }
    return _wifiMulti->run() == WL_CONNECTED;
}

void RzWifi::setup() {
    if (strlen(_ssid) == 0 || strlen(_password) == 0) {
        Serial.println(F("Wifi Station disabled."));
        WiFi.mode(WIFI_AP);
        initSoftAP();
        return;
    }

    Serial.println(F("Wifi Station enabled."));
    WiFi.mode(WIFI_AP_STA);
    initSoftAP();

    WiFi.scanNetworks(true, true); // Async and show hidden
    Serial.printf("Trying to connect to: %s ", _ssid);
    _wifiMulti->addAP(_ssid, _password);   // FIXME: support multiple networks

    int i = 40;   // 40 * 1/4 seconds = 10seconds;
    while (_wifiMulti->run() != WL_CONNECTED && i--) {
        // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
        delay(250);
        Serial.print('.');
    }
    if (_wifiMulti->run() == WL_CONNECTED) {
        Serial.println(F("Connection established!"));
//        Serial.print("Connected to ");
//        Serial.println(WiFi.SSID());              // Tell us what network we're connected to
        Serial.print(F("IP address:\t"));
        Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

        if (!MDNS.begin(_dnsName)) {             // Start the mDNS responder for esp8266.local
            Serial.println(F("Error setting up MDNS responder!"));
        }
        Serial.print(F("mDNS responder started: "));
        Serial.println(_dnsName);

        // NTP setup
        _timeClient->begin();
        // Sometimes the NTPClient retrieves the year of 1970. To ensure that doesnt happen we force the update.
        Serial.print(F("Trying to synchronizing time"));
        if (!_timeClient->update()) { // while
            Serial.print(".");
            _timeClient->forceUpdate();
        }
        _isTimeSynchronized = true;
        //timeClient->setTimeOffset(2);
        Serial.printf("\r\nTime is synchronized: %s\r\n", formatDate(getTime()).c_str());

    } else {
        Serial.println(F("Connection failed!"));
    }

//  Serial.println("Sending mDNS Query");
//  int n = MDNS.queryService("piscine", "tcp");

// if (n == 0) {
//    Serial.println("No service found");
//  }
//  else {
//    Serial.println("Service found");
//    Serial.println("Host: " + String(MDNS.hostname(0)));
//    Serial.print("IP  : " );
//    Serial.println(MDNS.IP(0));
//    Serial.println("Port: " + String(MDNS.port(0)));
//  }
}

void RzWifi::loop(timeMs referenceTime) {
    // TODO: add check for connection loss?
    MDNS.update();
    if (_isTimeSynchronized) {
        _timeClient->update();
    }
}


void RzWifi::initSoftAP() {
    //IPAddress local_IP(192, 168, 1, 1);
    //IPAddress gateway(192, 168, 1, 1);
    //IPAddress subnet(255, 255, 255, 0);

    uint16_t uChipId = ESP.getChipId();
    char accessPointName[20];
    int ret = snprintf(accessPointName, sizeof accessPointName, "ESP8266.4.1-%X", uChipId);
    if (ret < 0 || ret >= (int) sizeof accessPointName) {
        Serial.println(F("Error while generating accessPointName"));
    }

    char accessPointPassword[20];
    ret = snprintf(accessPointPassword, sizeof accessPointPassword, "cornichon-%X", uChipId);
    if (ret < 0 || ret >= (int) sizeof accessPointPassword) {
        Serial.printf("Error while generating accessPointPassword\r\n");
    }

    //Serial.print("Setting soft-AP configuration ... ");
    //Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

    Serial.print("Setting soft-AP ... ");
    Serial.println(WiFi.softAP(accessPointName, accessPointPassword) ? "Ready" : "Failed!");

    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());
}

timeMs RzWifi::getTime() {
    return isTimeSynchronized() ? _timeClient->getEpochTime() : MIN_TIMESTAMP + millis();
}

String RzWifi::formatDate(timeMs time) {
    if (time < MIN_TIMESTAMP) return String(); // we do not convert if not from EPOCH

    struct tm *ptm = gmtime((time_t *) &time);
    char buffer[25];
    int ret = snprintf(buffer, sizeof buffer, "%04d-%02d-%02dT%02d:%02d:%02dZ",  //
                       ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,       //
                       ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    if (ret < 0 || ret >= (int) sizeof buffer) {
        Serial.printf("Error while formatting date\r\n");
    }
    return String(buffer);
}

void RzWifi::loadConfiguration() {
    File file = getConfigurationFile(RzWifi::getPrefix(), true);

    if (file) {
        Serial.printf("Opening configuration: %s\r\n", file.fullName());
        _dnsName = file.readStringUntil('\n').c_str();
        _ssid = file.readStringUntil('\n').c_str();
        _password = file.readStringUntil('\n').c_str();
        _poolServerName = file.readStringUntil('\n').c_str();
        _timeOffset = file.readStringUntil('\n').toInt();
        _updateInterval = file.readStringUntil('\n').toInt();
    } else {
        Serial.printf("Configuration file for %s not found\r\n", RzWifi::getPrefix());
    }
    file.close();
}

void RzWifi::saveConfiguration() {
    File file = getConfigurationFile(RzWifi::getPrefix(), false);
    file.write(_dnsName);
    file.write(_ssid);
    file.write(_password);
    file.write(_poolServerName);
    file.write(_timeOffset);
    file.write(_updateInterval);
    file.close();
}


const char *RzWifi::getId() {
    return "net";
}

const char *RzWifi::getDisplayName() {
    return "Network Configuration";
}

const char *RzWifi::getPrefix() {
    return "Network";
}

#define NETWORK_CONFIG_MAX_SIZE 768

const char *RzWifi::getJsonConfig() {
    char *config = new char[NETWORK_CONFIG_MAX_SIZE+1];
    *config = 0;

    myTools::stringConcat(config, F(R"({"id":")"), NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, getId(), NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"(","title":")"), NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, getDisplayName(), NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"(","parameters": [)"), NETWORK_CONFIG_MAX_SIZE);

    // Wifi config
    myTools::stringConcat(config, F(R"({"id":"wifi","title":"Wifi","parameters": [)"), NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"({"name": "Dns Name","id": "dnsName","value":")"), NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, _dnsName, NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"lit("},{"name": "SSID","id": "ssid","value":")lit"), NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, _ssid, NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"("},{"name": "Password","id": "password","value":")"), NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, _password, NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"("}]})"), NETWORK_CONFIG_MAX_SIZE);

    // NTP configuration
    myTools::stringConcat(config, F(R"(,{"id":"ntp","title":"Network Synchronized Time","parameters": [)"), NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"({"name": "Pool Server Name","id": "poolServerName","value":")"), NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, _poolServerName, NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"lit("},{"name": "Time Offset (hours)","id": "timeOffset","value":)lit"), NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, _timeOffset, NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"lit(},{"name": "Update Interval (ms)","id": "updateInterval","value":)lit"), NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, _updateInterval, NETWORK_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"(}]})"), NETWORK_CONFIG_MAX_SIZE);

    myTools::stringConcat(config, F(R"(]})"), NETWORK_CONFIG_MAX_SIZE);

    return config;
}


//Address IP local_ip(192,168,1,1)
//Address IP gateway(192,168,1,1)
//Address IP subnetwork(255,255,255,0)
//Wifi.softAP(ssid, mot de passe);
//Wifi.softAPConfig(local_ip, gateway, subnetwork); // <- bridge to internet?!!

// https://github.com/tzapu/WiFiManager
// https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/
