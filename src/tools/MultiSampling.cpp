#include "MultiSampling.h"


MultiSampling::MultiSampling(const char *id, //
                             int nbSampling, int extremes, ulong delaySampling, ulong delayMeasure) :
        _id(id), _size(nbSampling), _extremes(extremes), _delaySampling(delaySampling), _delayMeasure(delayMeasure) {
    MultiSampling::loadConfiguration();
    _buf = new int[_size];
    clean();
}


void MultiSampling::clean() {
    _current = 0;
    _previousMeasure = 0;
    _previousSampling = 0;
}

void MultiSampling::add(timeMs currentMillis, int value) {
    _previousSampling = currentMillis;
    _buf[_current++] = value;
}

bool MultiSampling::needMore() const {
    return _current < _size;
}

bool MultiSampling::isReady(timeMs currentMillis) const {
    return _previousSampling == 0 || currentMillis - _previousSampling > _delaySampling;
}

int MultiSampling::getFinalValue() {
    for (int i = 0; i < _current - 1; i++) {
        for (int j = i + 1; j < _current; j++) {
            if (_buf[i] > _buf[j]) {
                int temp = _buf[i];
                _buf[i] = _buf[j];
                _buf[j] = temp;
            }
        }
    }
    int finalValue = 0;
    for (int i = _extremes; i < _current - _extremes; i++) {
        finalValue += _buf[i];
    }
    finalValue = finalValue / (_current - (2 * _extremes));

    return finalValue;
}

int MultiSampling::sample(timeMs currentMillis, const SamplingHandler &_lambda) {
    if (_previousMeasure > 0 && currentMillis - _previousMeasure < _delayMeasure) {
        return INT32_MIN; // Measures will have to wait
    }

    if (!needMore()) {
        int value = getFinalValue();
        clean();
        _previousMeasure = currentMillis;
        return value;
    } else if (isReady(currentMillis)) {
        int temp = _lambda();
        add(currentMillis, temp);
    }
    return INT32_MIN; // Measure sampling not finished
}

void MultiSampling::displayConfig() const {
    Serial.printf(
            "MultiSampling(nb sampling: %i, remove extremes: %i, delay between measures: %lu, delay between samplings: %lu)\r\n",
            _size, _extremes, _delayMeasure, _delaySampling);
}

String MultiSampling::getJsonConfig() {
    String config;
    config.reserve(340); // Do we need an id?
    config.concat(R"({"id":"multisampling","title":"Multi sampling","parameters": [)");
    config.concat(R"({"name": "Sampling number","id": "size","value":)");
    config.concat(_size);
    config.concat(R"(},{"name": "Remove extremes","id": "extremes","value":)");
    config.concat(_extremes);
    config.concat(R"lit(},{"name": "Delay between samples (ms)","id": "delaySampling","value":)lit");
    config.concat(_delaySampling);
    config.concat(R"lit(},{"name": "Delay between measures (ms)","id": "delayMeasures","value":)lit");
    config.concat(_delayMeasure);
    config.concat(R"(}]})");
    return config;
}

MultiSampling::~MultiSampling() = default;

void MultiSampling::loadConfiguration() {
    File file = getConfigurationFile(MultiSampling::getPrefix(), true);

    if (file) {
        Serial.printf("Opening configuration: %s\r\n", file.fullName());
        _size = file.readStringUntil('\n').toInt();
        _extremes = file.readStringUntil('\n').toInt();
        _delaySampling = file.readStringUntil('\n').toInt();
        _delayMeasure = file.readStringUntil('\n').toInt();
    } else {
        Serial.printf("Configuration file for %s (%s) not found\r\n", MultiSampling::getPrefix(), getId());
    }
    file.close();
}

void MultiSampling::saveConfiguration() {
    File file = getConfigurationFile(MultiSampling::getPrefix(), false);
    file.write(_size);
    file.write(_extremes);
    file.write(_delaySampling);
    file.write(_delayMeasure);
    file.close();
}

const char *MultiSampling::getId() {
    return _id;
}

const char *MultiSampling::getDisplayName() {
    return "Multisampling";
}

const char *MultiSampling::getPrefix() {
    return "Multisampling";
}
