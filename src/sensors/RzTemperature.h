#ifndef _TEMPERATURE_SENSOR_H_
#define _TEMPERATURE_SENSOR_H_

#include <OneWire.h>
#include <DallasTemperature.h>

#include "base/RzSensor.h"
#include "RzFiles.h"
#include "tools/LinearCorrection.h"
#include "tools/MultiSampling.h"

//#define TEMPERATURE_PRECISION 11

class RzTemperature : public RzSensor {
public:
    RzTemperature(uint8_t pin, const char *id, const char *displayName);

    //RzTemperature(RzFiles &files, uint8_t pin, DeviceAddress deviceAddress);

    void setup() override;

    void loop(timeMs referenceTime) override;

    String getJsonConfig() override;

    ~RzTemperature() override;

    const char *getDisplayName() override;

    const char *getPrefix() override;

    void loadConfiguration() override;

    void saveConfiguration() override;

private:
    const char *_displayName;

    OneWire *_oneWire;
    DallasTemperature *_sensors;

    // linear correction tool
    LinearCorrection *_linCor;
    // Multisampling configuration
    MultiSampling *_multisampling;
    // arrays to hold device addresses
    DeviceAddress _device{};

    float getTemperature();

    void printAddress();

};

#endif
