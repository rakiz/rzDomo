#include <Arduino.h>
#include "base/RzSensor.h"

RzSensor::RzSensor(const char *id, const char *unit, uint precision,
                   uint maxSize, ulong maxDelayValues, uint maxComparedValues, uint ignoredDiff) :
        _id(id), _unit(unit), _precision(precision),
        _maxSize(maxSize), _maxDelayValues(maxDelayValues), _maxComparedValues(maxComparedValues),
        _ignoredDiff(ignoredDiff) {
    RzSensor::loadConfiguration();
    _size = 0;
    _values = new MetricStruct[_maxSize];
}

RzSensor::~RzSensor() {
    delete _values;
}

void RzSensor::addValue(timeMs time, int value) {
    uint pos = _size;
    if (!hasValueToBeAdded(time, value)) {
        Serial.printf("[%lu] Replace previous value for '%s' with %d\r\n", time, getDisplayName(), value);
        pos--;  // new value and previous ones are similar, let's keep only the latest
    } else {
        if (_size < _maxSize) {
            Serial.printf("[%lu] Add value for '%s': %d\r\n", time, getDisplayName(), value);
            _size++; // there is still room for more measures
        } else {
            Serial.printf("[%lu] Make place to add value for '%s': %d\r\n", time, getDisplayName(), value);
            // there's no place anymore, let's move it all around
            for (uint i = 0; i < _maxSize - 1; i++) {
                _values[i] = _values[i + 1];
            }
        }
    }
    _values[pos].time = time;
    _values[pos].value = value;
}

bool RzSensor::hasValueToBeAdded(timeMs time, int value) {
    bool addNewValue = _size < 2 || time - _values[_size - 2].time >= _maxDelayValues;
//    Serial.printf("size: %d - deltaTime: %lu max: %lu -> result: %i\r\n",_size, time-_values[_size-2].time, _maxDelayValues, addNewValue);
    for (uint i = 0; !addNewValue && i < _maxComparedValues && i < _size; i++) {
        // TODO: utiliser pente de la courbe <??
        uint coef = i + 1;
        uint diff = abs(value - _values[_size - coef].value);
        uint diff_threshold = _ignoredDiff * coef;
        addNewValue |= (diff >= diff_threshold);
//        Serial.printf("value(%i)=%.2d local=%.2d => diff=%.2u / maxDif=%.2u / result=%i \r\n",
//            _size-coef, _values[_size-coef].value, value, diff, diff_threshold, addNewValue);
    }
    return addNewValue;
}

__unused double RzSensor::convertValue(int value) const {
    return (float) (value / pow(10, _precision));
}

int RzSensor::convertValue(double value) const {
    return lround(value * pow(10, _precision) + 0.5);
}


// -=[ RzConfigurable }=------------------------------------------------------

void RzSensor::loadConfiguration() {
    File file = getConfigurationFile(RzSensor::getPrefix(), true);

    if (file) {
        Serial.printf("Opening configuration: %s\r\n", file.fullName());
        _maxSize = file.readStringUntil('\n').toInt();
        _maxDelayValues = file.readStringUntil('\n').toInt();
        _maxComparedValues = file.readStringUntil('\n').toInt();
        _ignoredDiff = file.readStringUntil('\n').toInt();
    } else {
        Serial.printf("Configuration file for %s (%s) not found\r\n", RzSensor::getPrefix(), getId());
    }
    file.close();
}

void RzSensor::saveConfiguration() {
    File file = getConfigurationFile(RzSensor::getPrefix(), false);
    file.write(_maxSize);
    file.write(_maxDelayValues);
    file.write(_maxComparedValues);
    file.write(_ignoredDiff);
    file.close();
}

String RzSensor::getJsonConfig() {
    String config;
    config.reserve(512); // Do we need an id?
    config.concat(F(R"({"id":"sensor","title":"Sensor","parameters": [)"));
    config.concat(F(R"({"name": "Max kept values","id": "maxSize","value":)"));
    config.concat(_maxSize);
    config.concat(F(R"lit(},{"name": "Delay between measures (ms)","id": "maxDelayValues","value":)lit"));
    config.concat(_maxDelayValues);
    config.concat(F(R"(},{"name": "Max compared values","id": "maxComparedValues","value":)"));
    config.concat(_maxComparedValues);
    config.concat(F(R"(},{"name": "Ignored difference","id": "ignoredDiff","value":)"));
    config.concat(_ignoredDiff);
    config.concat(F(R"(},{"name": "Precision","id": "precision", "access":"readonly", "value":)"));
    config.concat(_precision);
    config.concat(F("}]}"));
    return config;
}

const char *RzSensor::getPrefix() {
    return "Sensor";
}

const char *RzSensor::getId() {
    return _id;
}
