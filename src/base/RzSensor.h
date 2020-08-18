#ifndef _RZ_SENSOR_H_
#define _RZ_SENSOR_H_

#include <Arduino.h>
#include "RzFiles.h"
#include "base/RzComponent.h"
#include "base/RzConfigurable.h"


// To be removed. Should be some kept in memory, others on disk in flat CSV files
//const uint MAX_METRIC_VALUES = 240;

// one hour max between measures
//const uint MAX_DELAY_BETWEEN_MEASURES = 60 * 60;

//const uint IGNORED_DIFFERENCE = 20;
//const uint MAX_VALUES_TO_COMPARE = 2;

/*
enum MetricValueType {
    INT, FLOAT, BOOL
};
union MetricValue {
    int intValue;
    float floatValue;
    bool boolValue;
};
*/

struct MetricStruct {
    timeMs time;
    int value;
};

class RzSensor : public RzComponent, public RzConfigurable {
public:
    RzSensor(const char* id, const char *unit, uint precision,
             uint maxSize, ulong maxDelayValues, uint maxComparedValues, uint ignoredDiff);

    virtual ~RzSensor() = 0;

    void addValue(timeMs time, int value);

    inline const char *getUnit() { return _unit; }

    int convertValue(double value) const;

    double convertValue(int value) const;

    inline uint getPrecision() const { return _precision; }

    // RzConfigurable -----------------------------------

    void loadConfiguration() override;

    void saveConfiguration() override;

    String getJsonConfig() override;

    const char *getId() override;

    const char *getPrefix() override;

    const char *getDisplayName() override = 0;

protected:
    const char *_id;
    const char *_unit;
    uint _precision;
    MetricStruct *_values;
    uint _size;
    // -=[ Configuration parameters ]=------------------------------
    uint _maxSize = 240;
    ulong _maxDelayValues = 60 * 60;
    uint _maxComparedValues = 2;
    uint _ignoredDiff = 20;
private:

    bool hasValueToBeAdded(timeMs _time, int _value);

protected:
};

#endif