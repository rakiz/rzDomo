#include "RzTemperature.h"
#include "../tools/LinearCorrection.h"

RzTemperature::RzTemperature(RzTime *_myTime, uint8_t _pin, unsigned long _interval) {
    myTime = _myTime;

    // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
    oneWire = OneWire(_pin);

    // Pass our oneWire reference to Dallas Temperature.
    sensors = DallasTemperature(&oneWire);

    // linear correction tool
    linCor = new LinearCorrection(0, 99.3, 0, 99.3);

    // Multisampler configuration
    multisampling = new MultiSampling(14, 2, 25, _interval);

    // for debuggin without sensors pluged in !!
//    addValue(1595320294, 25.34);
//    addValue(1595323835, 25.55);
//    addValue(1595327375, 25.78);
//    addValue(1595330916, 25.88);
//    addValue(1595337997, 26);
//    addValue(1595349970, 26.13);

}

void RzTemperature::setup() {
    // locate devices on the bus
    Serial.print("Locating devices...");
    sensors.begin();
    Serial.print("Found ");
    Serial.print(sensors.getDeviceCount(), DEC);
    Serial.println(" devices.");

    // report parasite power requirements
    Serial.print("Parasite power is: ");
    if (sensors.isParasitePowerMode()) Serial.println("ON");
    else Serial.println("OFF");

    oneWire.reset_search();
    // assigns the first address found to insideThermometer
    if (!oneWire.search(thermometer)) Serial.println("Unable to find address for thermometer");

    // show the addresses we found on the bus
    Serial.print("Device 0 Address: ");
    printAddress(thermometer);
    Serial.println();
    linCor->displayConfig();
    multisampling->displayConfig();

    // set the resolution to 9 bit per device
    //sensors.setResolution(thermometer, TEMPERATURE_PRECISION);

    Serial.print("Device 0 Resolution: ");
    Serial.print(sensors.getResolution(thermometer), DEC);
    Serial.println();
}

void RzTemperature::loop(unsigned long currentMillis) {
    SamplingHandler func = [this]() -> float {
        sensors.requestTemperatures();
        return getTemperature(thermometer);
    };

    float tempC = multisampling->sample(currentMillis, func);
    if (tempC >= 0) {
        addValue(myTime->getEpochTime(), tempC);
        //Serial.print("DONE - Temp C: "); Serial.println(tempC);
    }
}


String RzMetric::getId() const { return F("tp"); }

String RzMetric::getDisplayName() const { return F("Température Piscine"); }

String RzMetric::getUnit() const { return F("°C"); }

float RzTemperature::getTemperature(DeviceAddress deviceAddress) {
    float tempC = sensors.getTempC(deviceAddress);
    if (tempC == DEVICE_DISCONNECTED_C) {
        Serial.println("Error: Could not read temperature data");
        return -1;
    }
    return linCor->fixValue(tempC);
}


// function to print a device address
void RzTemperature::printAddress(DeviceAddress deviceAddress) {
    for (uint8_t i = 0; i < 8; i++) {
        // zero pad the address if necessary
        if (deviceAddress[i] < 16) Serial.print("0");
        Serial.print(deviceAddress[i], HEX);
    }
}
