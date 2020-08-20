#include "RzMemory.h"
#include "tools/Tools.h"

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

#define MEMORY_CONFIG_MAX_SIZE 512

const char *RzMemory::getJsonConfig() {
    char *config = new char[MEMORY_CONFIG_MAX_SIZE+1];
    *config = 0;

    myTools::stringConcat(config, F(R"({"id":")"), MEMORY_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, getId(), MEMORY_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"(","title":")"), MEMORY_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, getDisplayName(), MEMORY_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"(","parameters": [)"), MEMORY_CONFIG_MAX_SIZE);
    const char *sensorConfig = RzSensor::getJsonConfig();
    myTools::stringConcat(config, sensorConfig, MEMORY_CONFIG_MAX_SIZE);
    delete[] sensorConfig;
    myTools::stringConcat(config, F("]}"), MEMORY_CONFIG_MAX_SIZE);

    return config;
}
