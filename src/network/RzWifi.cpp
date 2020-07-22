#include <Arduino.h>
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <cstdint>

#include "RzWifi.h"

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'


RzWifi::RzWifi(String _dnsName, String _credentials[], int _size) {
    dnsName = _dnsName;
    credentials = _credentials;
    size = _size;
}

void RzWifi::setup() {
    WiFi.mode(WIFI_AP_STA);

    uint16_t uChipId = ESP.getChipId();
    char accessPointName[20];
    char accessPointPassword[20];
    sprintf(accessPointName, "ESP8266.4.1-%X", uChipId);
    sprintf(accessPointPassword, "cornichon-%X", uChipId);
    WiFi.softAP(accessPointName, accessPointPassword);

    WiFi.scanNetworks(true, true); // Async and show hidden
    Serial.println("Trying to connect to:");
    for (int i = 0; i < size; i++) {
        Serial.print("\t");
        Serial.println(credentials[i * 2]);
        wifiMulti.addAP(credentials[i * 2].c_str(), credentials[i * 2 + 1].c_str());
    }
    while (wifiMulti.run() !=
           WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
        delay(250);
        Serial.print('.');
    }

    Serial.println();
    Serial.println("Connection established!");
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());              // Tell us what network we're connected to
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

    if (!MDNS.begin(dnsName)) {             // Start the mDNS responder for esp8266.local
        Serial.println("Error setting up MDNS responder!");
    }
    Serial.print("mDNS responder started: ");
    Serial.println(dnsName);

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


//Adresse IP local_ip(192,168,1,1)
//Adresse IP passerelle(192,168,1,1)
//Adresse IP sous-réseau(255,255,255,0)
//Wifi.softAP(ssid, mot de passe);
//Wifi.softAPConfig(local_ip, passerelle, sous-réseau); // <- pont vers internet?!!

// https://github.com/tzapu/WiFiManager
// https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/

}

void RzWifi::loop(unsigned long currentMillis) {
    MDNS.update();
}