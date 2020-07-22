#include <ArduinoJson.h>
#include "../tools/Tools.h"
#include "RzServer.h"

RzServer::RzServer(int _port, RzTime *_myTime, RzFiles *_myFiles, RzMetric *_metric) {
    myServer = new ESP8266WebServer(_port);
    myTime = _myTime;
    myFiles = _myFiles;
    myMetric = _metric;
}

//void handleLED() {                          // If a POST request is made to URI /LED
//  digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));      // Change the state of the LED
//  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
//  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
//}


//void RzServer::handleMetrics() {
//    int nb = myMetric->getSize();
//    if (myServer->hasArg("last")) {
//        nb = min(nb, atoi(myServer->arg("last").c_str()));
//    }
//
//    MetricStruct *measures = &(myMetric->getValues()[myMetric->getSize() - nb]);
//
//    DynamicJsonDocument telemetry(32 * MAX_METRIC_VALUES); // TODO: replace ArduinoJson with simple streamed writes
//    telemetry.createNestedObject();
//
//    for (int i = 0; i < nb; i++) {
//        telemetry[i]["ts"] = measures->time;
//        telemetry[i][myMetric->getName()] = measures->value;
//        measures++;
//    }
//
//    String contentType = getContentType("metrics.json");              // Get the MIME type
//    myServer->sendHeader("Access-Control-Allow-Origin", "*");
//    myServer->send(200, contentType, telemetry.as<String>().c_str());
//    Serial.printf("%d metrics sent: (%s)\r\n", nb, contentType.c_str());
//    telemetry.clear();
//}

void RzServer::handleVersion() {
    DynamicJsonDocument version(100);
    version["version"] = "2002.07";
    String contentType = getContentType("metrics.json");              // Get the MIME type
    myServer->sendHeader("Access-Control-Allow-Origin", "*");
    myServer->send(200, contentType, version.as<String>().c_str());
    Serial.printf("Metrics sent: %s (%s)\r\n", version.as<String>().c_str(), contentType.c_str());
}

void RzServer::setup() {
    // HTTP server
    myServer->onNotFound([this]() {
        String path = myServer->uri();
        if (path.endsWith("/")) path += "index.html";           // If a folder is requested, send the index file
        File file = myFiles->fileRead(path);
        if (!file) {                 // send it if it exists
            Serial.println("\tFile Not Found: " + path);
            myServer->send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
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
    MetricStruct *measures = myMetric->getValues();   // start from the beginning
    int nb = myMetric->getSize();                     // number of value to return = size of array
    if (myServer->hasArg("since")) {
        // 'since' parameter contains the last timestamp the UI received
        unsigned long timestamp = (unsigned long)atol(myServer->arg("since").c_str());
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

    DynamicJsonDocument telemetry(32 * MAX_METRIC_VALUES); // TODO: replace ArduinoJson with simple streamed writes

    for (int i = 0; i < nb; i++) {
        telemetry[i]["ts"] = measures->time;
        telemetry[i][myMetric->getName()] = measures->value;
        measures++;
    }

    String contentType = getContentType("metrics.json");              // Get the MIME type
    myServer->sendHeader("Access-Control-Allow-Origin", "*");
    myServer->send(200, contentType, telemetry.as<String>().c_str());
    Serial.printf("%d metrics sent: (%s)\r\n", nb, contentType.c_str());
    telemetry.clear();
}



//  /api/metrics -> JSON with metrics
//  / -> web page

//  String json;
//  json.reserve(128);
//  json = "{\"type\":\"";
//  json += fsName;
//  json += "\"}";

//  / use HTTP/1.1 Chunked response to avoid building a huge temporary string
//  if (!server.chunkedResponseModeStart(200, "text/json")) {
//    server.send(505, F("text/html"), F("HTTP1.1 required"));
//    return;
//  }
//
//  // use the same string for every line
//  String output;
//  output.reserve(64);
//  while (dir.next()) {
//    if (output.length()) {
//      // send string from previous iteration
//      // as an HTTP chunk
//      server.sendContent(output);
//      output = ',';
//    } else {
//      output = '[';
//    }
//
//    output += "{\"type\":\"";
//    output += "\"}";
//  }
//
//  // send last string
//  output += "]";
//  server.sendContent(output);
//  server.chunkedResponseFinalize();
//
//  server.on("/edit", HTTP_GET, handleGetEdit);
//  server.on("/edit",  HTTP_PUT, handleFileCreate);
//  server.on("/edit",  HTTP_DELETE, handleFileDelete);
//  // Upload file
//  // - first callback is called after the request has ended with all parsed arguments
//  // - second callback handles file upload at that location
//  server.on("/edit",  HTTP_POST, replyOK, handleFileUpload);
//
// https://tttapa.github.io/ESP8266/Chap14%20-%20WebSocket.html