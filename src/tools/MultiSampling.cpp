#include "MultiSampling.h"


MultiSampling::MultiSampling(int _nbSampling, int _extrems, unsigned long _delaySampling, unsigned long _delayMeasure) {
    nbSampling = _nbSampling;
    extrems = _extrems;
    delaySampling = _delaySampling;
    delayMeasure = _delayMeasure;
    buf = new float[nbSampling];
    clean();
}


void MultiSampling::clean() {
    current = 0;
    previousMeasure = 0;
    previousSampling = 0;
}

void MultiSampling::add(unsigned long _currentMillis, float _value) {
    previousSampling = _currentMillis;
    buf[current++] = _value;
}

bool MultiSampling::needMore() const {
    return current < nbSampling;
}

bool MultiSampling::isReady(unsigned long _currentMillis) const {
    return previousSampling == 0 || _currentMillis - previousSampling > delaySampling;
}

float MultiSampling::getFinalValue() {
    for (int i = 0; i < current - 1; i++) {
        for (int j = i + 1; j < current; j++) {
            if (buf[i] > buf[j]) {
                float temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    float finalValue = 0;
    for (int i = extrems; i < current - extrems; i++) {
        finalValue += buf[i];
    }
    finalValue = finalValue / (float)(current - (2 * extrems));

    return finalValue;
}

float MultiSampling::sample(unsigned long _currentMillis, const SamplingHandler& _lambda) {
    if (previousMeasure > 0 && _currentMillis - previousMeasure < delayMeasure) {
        return -1; // Measures will have to wait
    }

    if (!needMore()) {
        float tempC = getFinalValue();
        clean();
        previousMeasure = _currentMillis;
        return tempC;
    } else if (isReady(_currentMillis)) {
        float temp = _lambda();
        add(_currentMillis, temp);
    }
    return -2; // Measure sampling not finished
}

void MultiSampling::displayConfig() {
    Serial.printf(
            "MultiSampling(nb sampling: %i, remove extrems: %i, delay between measures: %lu, delay between samplings: %lu)\r\n",
            nbSampling, extrems, delayMeasure, delaySampling);
}

