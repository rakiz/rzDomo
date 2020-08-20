#include "base/RzComponents.h"
#include "tools/Tools.h"

void RzComponents::add(RzComponent *component) {
    _components.push_back(component);
    component->setup();
}

void RzComponents::visitSensorValues(RzSensorValueVisitor &visitor) {
    for (auto &_component : _components) {
        if (myTools::instanceof<RzSensor>(_component)) {
            auto *sensor = (RzSensor *) _component;
            if (visitor.onTime(0)) {   // FIXME: I need to get the time
                visitor.onValue(sensor->RzComponent::getId(), 0,
                                sensor->getPrecision()); // FIXME: I need to get the value
            }
        }
    }
}

void RzComponents::visitSensorChartConfig(RzSensorChartConfigVisitor &visitor) {
    for (auto &_component : _components) {
        if (myTools::instanceof<RzSensor>(_component)) {
            auto *sensor = (RzSensor *) _component;
            visitor.visit(sensor->RzComponent::getId(), sensor->RzComponent::getDisplayName(), sensor->getUnit());
        }
    }
}

void RzComponents::visitConfigurable(RzConfigurableVisitor &visitor) {
    for (auto &_component : _components) {
        if (myTools::instanceof<RzConfigurable>(_component)) {
            auto *configurable = (RzConfigurable *) _component;
            const char *jsonConfig = configurable->getJsonConfig();
            size_t len = strlen(jsonConfig);
            if (len > 0) {
                Serial.printf("Component %s configurable (config size=%d).\r\n", _component->getDisplayName(), len);
                visitor.visit(jsonConfig);
            }
            delete[] jsonConfig;
        }
    }
}

void RzComponents::loop(timeMs referenceTime) {
    for (auto &_component : _components) {
        _component->loop(referenceTime);
    }
}