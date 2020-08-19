#include "RzTemperature.h"

RzTemperature::RzTemperature(uint8_t pin, const char *id, const char *displayName) :
        RzSensor(id, "°C", 2, 240, 60 * 60 * 1000, 1, 20),
        _displayName(displayName) {
//    Serial.println("RzTemperature ctor");

    RzTemperature::loadConfiguration();

    // default value defined, overridable through configuration
    _linCor = new LinearCorrection(id, 0, 99.6, 0.49, 99.4);

    _multisampling = new MultiSampling(id, 14, 2, 25, 60 * 1000);

    // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
    _oneWire = new OneWire(pin);

    // Pass our oneWire reference to Dallas Temperature.
    _sensors = new DallasTemperature(_oneWire);

    Serial.print("Locating devices...");
    _sensors->begin();

    Serial.print("Found ");
    Serial.print(_sensors->getDeviceCount(), DEC);
    Serial.println(" devices.");

    _oneWire->reset_search();
    // assigns the first address found to insideThermometer
    if (!_oneWire->search(_device)) Serial.println("Unable to find address for thermometer");
    else {
        // show the addresses we found on the bus
        Serial.print("Device 0 Address: ");
        printAddress();
    }
}

// TODO: select which address we are using here
//RzTemperature::RzTemperature(RzFiles &files, uint8_t pin, DeviceAddress deviceAddress) :
//        RzSensor(2, "°C", files, 240, 60 * 60, 1, 20) {
//    init(pin);
//    // copy address
//    for (int i = 0; i < 8; i++) _device[i] = deviceAddress[i];
//
//    if (_sensors->validAddress(_device)) {
//        Serial.print("Valid device found at Address: ");
//    } else {
//        Serial.print("ERROR: Invalid device found at Address: ");
//    }
//    printAddress();
//}

RzTemperature::~RzTemperature() {
    delete _sensors;
    delete _oneWire;
    delete _linCor;
    delete _multisampling;
}

void RzTemperature::loadConfiguration() {
    // no configuration
}

void RzTemperature::saveConfiguration() {
    RzSensor::saveConfiguration();
}

/*
    // linear correction tool
    linCor = new LinearCorrection(0, 99.3, 0, 99.3);
    multisampling = new MultiSampling(14, 2, 25, _interval);
*/
void RzTemperature::setup() {
    // locate devices on the bus
    Serial.print("Locating devices...");
    _sensors->begin();
    Serial.print("Found ");
    Serial.print(_sensors->getDeviceCount(), DEC);
    Serial.println(" devices.");

    // report parasite power requirements
    Serial.print("Parasite power is: ");
    if (_sensors->isParasitePowerMode()) Serial.println("ON");
    else Serial.println("OFF");

    _oneWire->reset_search();
    // assigns the first address found to insideThermometer
    if (!_oneWire->search(_device)) Serial.println("Unable to find address for thermometer");

    // show the addresses we found on the bus
    Serial.print("Device 0 Address: ");
    printAddress();
    Serial.println();
    _linCor->displayConfig();
    _multisampling->displayConfig();

    // set the resolution to 9 bit per device
    //sensors.setResolution(thermometer, TEMPERATURE_PRECISION);

    Serial.print("Device 0 Resolution: ");
    Serial.print(_sensors->getResolution(_device), DEC);
    Serial.println();
}

void RzTemperature::loop(ulong referenceTime) {
    SamplingHandler func = [this]() -> int {
        _sensors->requestTemperatures();
        return convertValue(getTemperature());
    };

    int tempC = _multisampling->sample(millis(), func);
    if (tempC > INT32_MIN) {
        addValue(referenceTime, tempC);
        //Serial.print("DONE - Temp C: "); Serial.println(tempC);
    }
}

const char *RzTemperature::getDisplayName() {
    return _displayName;
}

const char *RzTemperature::getPrefix() {
    return "Temperature";
}

float RzTemperature::getTemperature() {
    float tempC = _sensors->getTempC(_device);
    if (tempC == DEVICE_DISCONNECTED_C) {
        Serial.println("Error: Could not read temperature data");
        return -1;
    }
    return _linCor->fixValue(tempC);
}


// function to print a device address
void RzTemperature::printAddress() {
    for (unsigned char i : _device) {
        // zero pad the address if necessary
        if (i < 16) Serial.print("0");
        Serial.print(i, HEX);
    }
}

String RzTemperature::getJsonConfig() {
    String config;
    config.reserve(1000);
    config += R"({"id": ")";
    config += getId();
    config += R"(","title": ")";
    config += "Température piscine";
    config += R"(","parameters": [)";
    config += _linCor->getJsonConfig();
    config += ",";
    config += _multisampling->getJsonConfig();
    config += "]}";
    return config;
}
