#include "RzMemory.h"

RzMemory::RzMemory() : RzSensor("mem", "Bytes", 0, 240, 60 * 60 * 1000, 1, 1000) {
    RzMemory::loadConfiguration();

    _multisampling = new MultiSampling("mem", 14, 2, 25, 60 * 1000);
}

RzMemory::~RzMemory() {
    delete _multisampling;
}

void RzMemory::setup() {
    // nothing to do
}

void RzMemory::loop(timeMs referenceTime) {
    SamplingHandler func = []() -> int {
        //ESP.getFreeHeap()
        uint16_t hMax;
        ESP.getHeapStats(nullptr, &hMax, nullptr);
        return hMax;
    };

    int value = _multisampling->sample(millis(), func);
    if (value > INT32_MIN) {
        addValue(referenceTime, value);
        //Serial.print("DONE - Temp C: "); Serial.println(tempC);
    }
}

const char *RzMemory::getDisplayName() {
    return "ESP Memory";
}

const char *RzMemory::getPrefix() {
    return "Memory";
}

void RzMemory::loadConfiguration() {
    // no configuration
}

void RzMemory::saveConfiguration() {
    RzSensor::saveConfiguration();
}

String RzMemory::getJsonConfig() {
    String config;
    config.reserve(2048); // TODO: verify this

    config.concat(R"({"id":")");
    config.concat(getId());
    config.concat(R"(","title":")");
    config.concat(getDisplayName());
    config.concat(R"(","parameters": [)");
    config.concat(RzSensor::getJsonConfig());
    config.concat("]}");

    return config;
}
