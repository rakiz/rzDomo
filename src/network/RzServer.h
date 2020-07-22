#ifndef _MYSERVER_H_
#define _MYSERVER_H_

#include <Arduino.h>
#include <ESP8266WebServer.h>   // Include the WebServer library
#include "../network/RzTime.h"
#include "../sensors/RzMetric.h"
#include "../RzFiles.h"
#include "RzTime.h"

#define SRV_PORT 80

class RzServer {
public:
    RzServer(int _port, RzTime *_myTime, RzFiles *_myFiles, RzMetric *_metric);

    void setup();

    void loop(unsigned long _currentMillis);

private:
    RzTime *myTime;
    RzFiles *myFiles;
    RzMetric *myMetric;
    ESP8266WebServer *myServer;

    void handleMetrics();

    void handleVersion();

};

#endif