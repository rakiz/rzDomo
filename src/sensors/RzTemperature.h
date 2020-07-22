#ifndef _TEMPERATURE_SENSOR_H_
#define _TEMPERATURE_SENSOR_H_

#include <OneWire.h>
#include <DallasTemperature.h>

#include "../network/RzTime.h"
#include "../tools/LinearCorrection.h"
#include "../tools/MultiSampling.h"
#include "RzMetric.h"

#define TEMPERATURE_PRECISION 11

class RzTemperature : public RzMetric {
public:
    RzTemperature(RzTime *myTime, uint8_t _pin, unsigned long _interval);

    void setup();

    void loop(unsigned long _currentMillis);

private:
    OneWire oneWire;
    DallasTemperature sensors;

    RzTime *myTime;
    // linear correction tool
    LinearCorrection *linCor;
    // Multisampler configuration
    MultiSampling *multisampling;
    // arrays to hold device addresses
    DeviceAddress thermometer;

    float getTemperature(DeviceAddress _deviceAddress);

    void printAddress(DeviceAddress);

};

#endif
