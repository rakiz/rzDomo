#include <Arduino.h>
#include "LinearCorrection.h"
#include "tools/Tools.h"

LinearCorrection::LinearCorrection(const char *id, //
                                   float refLow, float refHigh, float rawLow, float rawHigh) :
        _id(id), _refLow(refLow), _refHigh(refHigh), _rawLow(rawLow), _rawHigh(rawHigh) {
    LinearCorrection::loadConfiguration();
    _refRange = refHigh - refLow;
    _rawRange = rawHigh - rawLow;
}

float LinearCorrection::fixValue(float rawValue) const {
    return (((rawValue - _rawLow) * _refRange) / _rawRange) + _refLow;
}

void LinearCorrection::displayConfig() const {
    Serial.printf("LinearCorrection(refLow: %.2f, rawLow: %.2f, refHigh: %.2f, rawHigh: %.2f)\r\n", //
                  _refLow, _rawLow, _refHigh, _rawHigh);
}

#define LINEAR_CORRECTION_CONFIG_MAX_SIZE 512

const char *LinearCorrection::getJsonConfig() {
    char *config = new char[LINEAR_CORRECTION_CONFIG_MAX_SIZE+1];
    *config = 0;

    myTools::stringConcat(config, F(R"({"id":"lincor", "title":"Linear correction","parameters": [)"),  LINEAR_CORRECTION_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"({"name": "Reference low","id": "refLow","value":)"), LINEAR_CORRECTION_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, _refLow, LINEAR_CORRECTION_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"(},{"name": "Measured low","id": "rawLow","value":)"), LINEAR_CORRECTION_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, _rawLow, LINEAR_CORRECTION_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"(},{"name": "Reference high","id": "refHigh","value":)"), LINEAR_CORRECTION_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, _refHigh, LINEAR_CORRECTION_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"(},{"name": "Measured high","id": "rawHigh","value":)"), LINEAR_CORRECTION_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, _rawHigh, LINEAR_CORRECTION_CONFIG_MAX_SIZE);
    myTools::stringConcat(config, F(R"(}]})"), LINEAR_CORRECTION_CONFIG_MAX_SIZE);

    return config;
}

void LinearCorrection::loadConfiguration() {
    File file = getConfigurationFile(LinearCorrection::getPrefix(), true);

    if (file) {
        Serial.printf("Opening configuration: %s\r\n", file.fullName());
        _refLow = file.readStringUntil('\n').toFloat();
        _refHigh = file.readStringUntil('\n').toFloat();
        _rawLow = file.readStringUntil('\n').toFloat();
        _rawHigh = file.readStringUntil('\n').toFloat();
    } else {
        Serial.printf("Configuration file for %s (%s) not found\r\n", LinearCorrection::getPrefix(), getId());
    }
    file.close();
}

void LinearCorrection::saveConfiguration() {
    File file = getConfigurationFile(LinearCorrection::getPrefix(), false);
    file.write(String(_refLow).c_str());
    file.write(String(_refHigh).c_str());
    file.write(String(_rawLow).c_str());
    file.write(String(_rawHigh).c_str());
    file.close();
}

const char *LinearCorrection::getId() {
    return _id;
}

const char *LinearCorrection::getDisplayName() {
    return "Linear correction";
}

const char *LinearCorrection::getPrefix() {
    return "LinearCorrection";
}

LinearCorrection::~LinearCorrection() = default;
