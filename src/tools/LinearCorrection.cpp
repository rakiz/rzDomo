#include <Arduino.h>
#include "LinearCorrection.h"

LinearCorrection::LinearCorrection(float _refLow, float _refHigh, float _rawLow, float _rawHigh) {
    refLow = _refLow;
    refHigh = _refHigh;
    rawLow = _rawLow;
    rawHigh = _rawHigh;
    rawRange = _rawHigh - _rawLow;
    refRange = _refHigh - _refLow;
}

float LinearCorrection::fixValue(float _rawValue) const {
    return (((_rawValue - rawLow) * refRange) / rawRange) + refLow;
}

void LinearCorrection::displayConfig() {
    Serial.printf("LinearCorrection(refLow: %.2f, rawLow: %.2f, refHigh: %.2f, rawHigh: %.2f)\r\n", refLow, rawLow,
                  refHigh, rawHigh);
}
