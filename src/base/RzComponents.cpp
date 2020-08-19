#include "base/RzComponents.h"
#include "tools/Tools.h"

void RzComponents::add(RzComponent *component) {
    _components.push_back(component);
    component->setup();
}

void RzComponents::visitSensorValues(RzSensorValueVisitor &visitor) {
    for (auto &_component : _components) {
        if (instanceof<RzSensor>(_component)) {
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
        if (instanceof<RzSensor>(_component)) {
            auto *sensor = (RzSensor *) _component;
            visitor.visit(sensor->RzComponent::getId(), sensor->RzComponent::getDisplayName(), sensor->getUnit());
        }
    }
}

void RzComponents::visitConfigurable(RzConfigurableVisitor &visitor) {
    for (auto &_component : _components) {
        if (instanceof<RzConfigurable>(_component)) {
            Serial.printf("Component %s configurable.\r\n", _component->getDisplayName());
            auto *configurable = (RzConfigurable *) _component;
            visitor.visit(configurable->getJsonConfig());
        }
    }
}

void RzComponents::loop(timeMs referenceTime) {
    for (auto &_component : _components) {
        _component->loop(referenceTime);
    }
}