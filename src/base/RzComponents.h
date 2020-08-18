#ifndef _RZ_COMPONENTS_H_
#define _RZ_COMPONENTS_H_

#include <Arduino.h>

#include "base/RzComponent.h"
#include "base/RzSensor.h"
#include "base/RzConfigurable.h"

typedef std::function<void(RzComponent&)> ComponentHandler;
//typedef std::function<void(RzSensorValueVisitor&)> SensorValueHandler;
//typedef std::function<void(RzConfigurable&)> ConfigurableVisitor;


class RzSensorValueVisitor {
public:
    virtual bool onTime(timeMs time) = 0;

    virtual void onValue(String id, int value, int precision) = 0;

};

class RzSensorChartConfigVisitor {
public:
    virtual void visit(const char *id, const char *displayName, const char *unit) = 0;
};

class RzConfigurableVisitor {
public:
    virtual void visit(String jsonConfig) = 0;
};


class RzComponents {
public:
    void visitSensorValues(RzSensorValueVisitor& visitor);
    void visitSensorChartConfig(RzSensorChartConfigVisitor& visitor);
    void visitConfigurables(RzConfigurableVisitor& visitor);

//    void setup();
    void loop(timeMs currentMillis);

    void add(RzComponent *component);
private:
    std::vector<RzComponent *> _components;
};

#endif
