#include "base/RzComponents.h"
#include "tools/Tools.h"

void RzComponents::add(RzComponent *component) {
    _components.push_back(component);
    component->setup();
}

void RzComponents::add(RzSensor *sensor) {
    _sensors.push_back(sensor);
    _components.push_back(sensor);
    sensor->setup();
}

void RzComponents::visitSensorValues(RzSensorValueVisitor &visitor) {
    for (auto &sensor : _sensors) {
        Serial.printf("Component %s is a Sensor.\r\n", sensor->getDisplayName());
        if (visitor.onTime(0)) {   // FIXME: I need to get the time
            visitor.onValue(sensor->getId(), 0, sensor->getPrecision()); // FIXME: I need to get the value
        }
    }
}

void RzComponents::visitSensorChartConfig(RzSensorChartConfigVisitor &visitor) {
    for (auto &sensor : _sensors) {
        Serial.printf("Component %s is a Sensor.\r\n", sensor->getDisplayName());
        visitor.visit(sensor->getId(), sensor->getDisplayName(), sensor->getUnit());
    }
}

void RzComponents::visitConfigurable(RzConfigurableVisitor &visitor) {
    for (auto &_component : _components) {
        const char *jsonConfig = _component->getJsonConfig();
        size_t len = strlen(jsonConfig);
        if (len > 0) {
            Serial.printf("Component %s is configurable (config size=%zu).\r\n", _component->getDisplayName(), len);
            visitor.visit(jsonConfig);
        }
        delete[] jsonConfig;
    }
}

void RzComponents::loop(timeMs referenceTime) {
    for (auto &_component : _components) {
        _component->loop(referenceTime);
    }
}