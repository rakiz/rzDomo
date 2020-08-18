#include "base/RzComponent.h"
#include "base/RzSensor.h"
#include "network/RzServer.h"
#include "tools/Tools.h"

RzServer::RzServer(int port, RzComponents &components) : _components(components){
    _server = new ESP8266WebServer(port);
}

RzServer::~RzServer() {
    delete _server;
}

void RzServer::setup() {
// HTTP server
    Serial.println("Setting up web site.");
    _server->onNotFound([this]() {
//        Serial.println("File access.");
        String path = _server->uri();
        if (path.endsWith("/")) path += "index.html";           // If a folder is requested, send the index file
        File file = RzFiles::openRead(path);
        if (!file) {                 // send it if it exists
            Serial.println("\tFile Not Found: " + path);
            _server->send(404, CONTENT_TYPE_TEXT, "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
        } else {
            String contentType = getContentType(path);             // Get the MIME type
            size_t sent = _server->streamFile(file, contentType);    // Send it to the client
            file.close();                                          // Close the file again
            Serial.printf("File sent: %s, size: %s (%s)\r\n", path.c_str(), formatBytes(sent).c_str(),
                          contentType.c_str());
        }
    });
    Serial.println("Setting up /api/metrics API.");
    _server->on("/api/metrics", HTTP_ANY, [this] {
//        Serial.println("/api/metrics API access.");
        handleMetrics();
    });
    Serial.println("Setting up /api/config API.");
    _server->on("/api/config", HTTP_ANY, [this] {
//        Serial.println("/api/config API access.");
        handleComponentConfig();
    });

    _server->begin();                           // Actually start the server
    Serial.print("HTTP server started on port: ");
    Serial.println(SRV_PORT);
}

void RzServer::loop(timeMs referenceTime) {
    _server->handleClient();                    // Listen for HTTP requests from clients
}

//[{"ts":1594376196,"t":27}]
void RzServer::handleMetrics() {
    _server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    if (_server->method() == HTTP_OPTIONS) {
        Serial.printf("OPTIONS(%d) %s\r\n", _server->method(), _server->uri().c_str());
        _server->sendHeader("Access-Control-Allow-Methods", "GET,OPTIONS");
        _server->send(200);
        return;
    } else if (_server->method() != HTTP_GET) {
        Serial.printf("UNSUPPORTED(%d) %s\r\n", _server->method(), _server->uri().c_str());
        _server->send(401);
        return;
    } else if (!_server->chunkedResponseModeStart(200, CONTENT_TYPE_JSON)) {
        // use HTTP/1.1 Chunked response to avoid building a huge temporary string
        _server->send(505, CONTENT_TYPE_HTML, F("HTTP1.1 required"));
        return;
    }

    // check first if we need metrics configuration
    if (_server->hasArg("config")) {
        uint nb = sendChartsConfig();
        Serial.printf("%d metric chart configurations sent\r\n", nb);
    } else {
        uint nb = sendMetrics();
        Serial.printf("%d metrics sent\r\n", nb);
    }
    _server->chunkedResponseFinalize();
}

uint RzServer::sendMetrics() {
    struct ValueVisitor : public RzSensorValueVisitor {
    public:
        ValueVisitor(ESP8266WebServer &server, timeMs since) :
                _buf{}, _count(0), _since(since), _currentTime(0), _server(server) {
        }

        virtual ~ValueVisitor() = default;

        bool onTime(timeMs time) override {
            if (time <= _since) return false;
            if (time != _currentTime && _currentTime != 0) {
                _server.sendContent("},"); // we have to finish previous bloc
            }
            _server.sendContent("{\"ts\":");
            sprintf(_buf, "%lu", time);
            _server.sendContent(_buf);
            _count++;
            return true;
        }

        void onValue(String id, int value, int precision) override {
            _server.sendContent(",\"");
            _server.sendContent(id);
            _server.sendContent("\":");
            if (precision == 0) {
                sprintf(_buf, "%d", value);
            } else {
                // FIXME: precision fixed a 2 + no bool support
                sprintf(_buf, "%.2f", (float) value / pow(10, precision));
            }
            _server.sendContent(_buf);
        }

        uint getCount() const {
            return _count;
        }

    private:
        char _buf[20];  // shared temporary buffer
        uint _count;
        timeMs _since;
        timeMs _currentTime;
        ESP8266WebServer &_server;
    };

    timeMs since = _server->hasArg("since") ? (timeMs) atol(_server->arg("since").c_str()) : 0;
    auto *pVisitor = new ValueVisitor(*_server, since);
    _server->sendContent("[");
    _components.visitSensorValues(*pVisitor);
    uint nb = pVisitor->getCount();
    if (nb > 0) _server->sendContent("}");
    _server->sendContent("]");
    delete pVisitor;

    return nb;
}

uint RzServer::sendChartsConfig() {
    struct ChartConfigVisitor : public RzSensorChartConfigVisitor {
    public:
        explicit ChartConfigVisitor(ESP8266WebServer &server) : _count(0), _server(server) {}

        virtual ~ChartConfigVisitor() = default;

        void visit(const char *id, const char *displayName, const char *unit) override {
            if (_count > 0) _server.sendContent(",");
            _server.sendContent(R"({"id":")");
            _server.sendContent(id);
            _server.sendContent(R"(","name":")");
            _server.sendContent(displayName);
            _server.sendContent(R"(","unit":")");
            _server.sendContent(unit);
            _server.sendContent("\"}");
            _count++;
        }

        uint getCount() const {
            return _count;
        }

    private:
        uint _count;
        ESP8266WebServer &_server;

    };

    auto *pVisitor = new ChartConfigVisitor(*_server);
    _server->sendContent("[");
    _components.visitSensorChartConfig(*pVisitor);
    uint nb = pVisitor->getCount();
    _server->sendContent("]");
    delete pVisitor;

    return nb;

}

void RzServer::handleComponentConfig() {
    for (int i = 0; i < _server->args(); i++) {
        Serial.printf("%s => %s\r\n", _server->argName(i).c_str(), _server->arg(i).c_str());
    }

    _server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    if (_server->method() == HTTP_POST) {
        Serial.printf("POST(%d) %s\r\n", _server->method(), _server->uri().c_str());
        // TODO pass configuration to the targeted component
        _server->send(201);
    } else if (_server->method() == HTTP_DELETE) {
        Serial.printf("DELETE(%d) %s\r\n", _server->method(), _server->uri().c_str());
        // TODO remove  configuration from targeted component
        _server->send(202);
    } else if (_server->method() == HTTP_OPTIONS) {
        Serial.printf("OPTIONS(%d) %s\r\n", _server->method(), _server->uri().c_str());
        _server->sendHeader("Access-Control-Allow-Methods", "GET,POST,DELETE,OPTIONS");
        _server->send(200);
        return;
    } else if (_server->method() != HTTP_GET) {
        Serial.printf("UNSUPPORTED(%d) %s\r\n", _server->method(), _server->uri().c_str());
        _server->send(401);
        return;
    } else if (!_server->chunkedResponseModeStart(200, CONTENT_TYPE_JSON)) {
        // use HTTP/1.1 Chunked response to avoid building a huge temporary string
        _server->send(505, CONTENT_TYPE_HTML, F("HTTP1.1 required"));
        return;
    }

    Serial.printf("GET(%d) %s\r\n", _server->method(), _server->uri().c_str());
    uint nb = sendComponentConfig();
    Serial.printf("%d component configurations sent\r\n", nb);
    _server->chunkedResponseFinalize();
}

uint RzServer::sendComponentConfig() {
    struct ConfigVisitor : public RzConfigurableVisitor {
    public:
        explicit ConfigVisitor(ESP8266WebServer &server) : _count(0), _server(server) {}

        virtual ~ConfigVisitor() {}

        void visit(String jsonConfig) override {
            if (_count > 0) _server.sendContent(",");
            _server.sendContent(jsonConfig);
            _count++;
        }

        uint getCount() const {
            return _count;
        }

    private:
        uint _count;
        ESP8266WebServer &_server;

    };

    auto *pVisitor = new ConfigVisitor(*_server);
    _server->sendContent("[");
    _components.visitConfigurables(*pVisitor);
    uint nb = pVisitor->getCount();
    _server->sendContent("]");
    delete pVisitor;

    return nb;
}


const char *RzServer::getId() {
    return "web";
}

const char *RzServer::getDisplayName() {
    return "Web server";
}

const char *RzServer::getPrefix() {
    return "Web";
}
//  /api/metrics -> JSON with metrics
//  / -> web page

//  server.on("/edit", HTTP_GET, handleGetEdit);
//  server.on("/edit",  HTTP_PUT, handleFileCreate);
//  server.on("/edit",  HTTP_DELETE, handleFileDelete);
//  // Upload file
//  // - first callback is called after the request has ended with all parsed arguments
//  // - second callback handles file upload at that location
//  server.on("/edit",  HTTP_POST, replyOK, handleFileUpload);
//
// https://tttapa.github.io/ESP8266/Chap14%20-%20WebSocket.html