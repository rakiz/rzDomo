#ifndef _RZ_SERVER_H_
#define _RZ_SERVER_H_

#include <Arduino.h>
#include <ESP8266WebServer.h>   // Include the WebServer library

#include "base/RzComponents.h"
#include "RzFiles.h"

#define SRV_PORT 80

static const char *const INDEX_HTML = "index.html";

class RzServer : public RzComponent {
public:
    RzServer(int port, RzComponents &components);

    virtual ~RzServer();

    void setup() override;

    void loop(timeMs referenceTime) override;

    const char *getId() override;

    const char *getDisplayName() override;

    const char *getPrefix() override;

    const char *getJsonConfig() override;

    void loadConfiguration() override;

    void saveConfiguration() override;

private:
    RzComponents &_components;
    ESP8266WebServer *_server;

    void handleMetrics();

    uint sendMetrics();

    uint sendChartsConfig();

    void handleComponentConfig();

    uint sendComponentConfig();
};

#endif