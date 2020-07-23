#ifndef _MY_METRIC_H_
#define _MY_METRIC_H_

#include <Arduino.h>
#include "../RzLoop.h"

struct MetricStruct {
    unsigned long time;
    float value;
};

// 01/01/2020T00:00:00
#define MIN_TIMESTAMP 1577836800

// one hour max between measures
#define MAX_DELAY_BETWEEN_MEASURES 60*60
// 240 measures = up to 10 days
#define MAX_METRIC_VALUES 240

#define IGNORED_DIFFERENCE 0.2
#define MAX_VALUES_TO_COMPARE 2

class RzMetric : public RzLoop {
public:
    RzMetric();

    virtual String getId() const;

    virtual String getDisplayName() const;

    virtual String getUnit() const;

    MetricStruct *getValues();

    int getSize() const;

    void addValue(unsigned long _time, float _value);

protected:
    MetricStruct values[MAX_METRIC_VALUES];
    int size = 0;
private:
    bool hasValueToBeAdded(unsigned long _time, float _value);
};

#endif