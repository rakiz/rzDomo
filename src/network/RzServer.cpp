#include "../tools/Tools.h"
#include "RzServer.h"

RzServer::RzServer(int _port, RzTime *_myTime, RzFiles *_myFiles, RzMetric *_metric) {
    myServer = new ESP8266WebServer(_port);
    myTime = _myTime;
    myFiles = _myFiles;
    myMetric = _metric;
}

void RzServer::handleVersion() {
    String version = F("{\"version\":\"2002.07\"}");
    myServer->sendHeader("Access-Control-Allow-Origin", "*");
    myServer->send(200, CONTENT_TYPE_JSON, version);
    Serial.printf("Metrics sent: %s \r\n", version.c_str());
}

void RzServer::setup() {
    // HTTP server
    myServer->onNotFound([this]() {
        String path = myServer->uri();
        if (path.endsWith("/")) path += "index.html";           // If a folder is requested, send the index file
        File file = myFiles->fileRead(path);
        if (!file) {                 // send it if it exists
            Serial.println("\tFile Not Found: " + path);
            myServer->send(404, CONTENT_TYPE_TEXT, "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
        } else {
            String contentType = getContentType(path);             // Get the MIME type
            size_t sent = myServer->streamFile(file, contentType);    // Send it to the client
            file.close();                                          // Close the file again
            Serial.printf("File sent: %s, size: %s (%s)\r\n", path.c_str(), formatBytes(sent).c_str(),
                          contentType.c_str());
        }
    });
    myServer->on("/api/version", HTTP_GET, [this] { handleVersion(); });
    myServer->on("/api/metrics", HTTP_GET, [this] { handleMetrics(); });

    myServer->begin();                           // Actually start the server
    Serial.print("HTTP server started on port: ");
    Serial.println(SRV_PORT);
}

void RzServer::loop(unsigned long _currentMillis) {
    myServer->handleClient();                    // Listen for HTTP requests from clients
}

//[{"ts":1594376196,"t":27}]
void RzServer::handleMetrics() {
    // use HTTP/1.1 Chunked response to avoid building a huge temporary string
    if (!myServer->chunkedResponseModeStart(200, CONTENT_TYPE_JSON)) {
        myServer->send(505, CONTENT_TYPE_HTML, F("HTTP1.1 required"));
        return;
    }

    MetricStruct *measures = myMetric->getValues();   // start from the beginning
    int nb = myMetric->getSize();                     // number of value to return = size of array
    if (myServer->hasArg("since")) {
        // 'since' parameter contains the last timestamp the UI received
        unsigned long timestamp = (unsigned long) atol(myServer->arg("since").c_str());
        // so we discard all metrics before (or equal) that timestamp
        for (int i = 0; i < myMetric->getSize() && measures->time <= timestamp; i++) {
            measures++;
            nb--;
        }
    } else if (myServer->hasArg("last")) {
        // 'last' parameter contains the number of metrics the UI wants. It will return only these metrics
        nb = min(nb, atoi(myServer->arg("last").c_str()));
        measures = &(myMetric->getValues()[myMetric->getSize() - nb]);
    }

    myServer->sendHeader(F("Access-Control-Allow-Origin"), F("*"));

    // use the same string for every line
    String output;
    output.reserve(64);
    output = '[';
    for (int i = 0; i < nb; i++) {
        if (output.length()>1) {
            // send string from previous iteration as an HTTP chunk
            myServer->sendContent(output);
            output = ',';
        }
        //{"ts":1595355283,"t":26.13}
        output += "{\"ts\":";
        output += measures->time;
        output += ",\"";
        output += myMetric->getName();
        output += "\":";
        output += measures->value;
        output += "}";
        measures++;
    }
    // send last string
    output += "]";
    myServer->sendContent(output);
    myServer->chunkedResponseFinalize();

    Serial.printf("%d metrics sent\r\n", nb);
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