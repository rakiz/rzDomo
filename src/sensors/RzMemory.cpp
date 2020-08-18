#include "RzMemory.h"

RzMemory::RzMemory() : RzSensor("mem", "Bytes", 0, 240, 60*60*1000, 1, 1000) {
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
        uint16_t hmax;
        ESP.getHeapStats(nullptr, &hmax,nullptr);
        return hmax;
    };

    int value = _multisampling->sample(millis(), func);
    if (value > INT32_MIN) {
        addValue(referenceTime, value);
        //Serial.print("DONE - Temp C: "); Serial.println(tempC);
    }
}

const char *RzMemory::getDisplayName() {
    return "Node memory";
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
    return RzSensor::getJsonConfig();
}
