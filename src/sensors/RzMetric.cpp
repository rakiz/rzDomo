#include "RzMetric.h"

RzMetric::RzMetric() {
    size = 0;
    for (int i = 0; i < MAX_METRIC_VALUES; i++) {
        values[i] = MetricStruct{};

    }
}

void RzMetric::addValue(unsigned long _time, float _value) {
    if (_time < MIN_TIMESTAMP) return;   // avoid dumb values

    float localValue = (float) ((int) (_value * 100 + .5)) / 100;

    int pos = size;
    if (!hasValueToBeAdded(_time, localValue)) {
        Serial.printf("Replace previous value for '%s' with %.2f\r\n", getDisplayName().c_str(), localValue);
        pos--;  // new value and previous ones are similar, let's keep only the latest
    } else {
        if (size < MAX_METRIC_VALUES) {
            Serial.printf("Add value for '%s': %.2f\r\n", getDisplayName().c_str(), localValue);
            size++; // there is still room for more measures
        } else {
            Serial.printf("Make place to add value for '%s': %.2f\r\n", getDisplayName().c_str(), localValue);
            // there's no place anymore, let's move it all around
            for (int i = 0; i < MAX_METRIC_VALUES - 1; i++) {
                values[i] = values[i + 1];
            }
        }
    }

    values[pos].time = _time;
    values[pos].value = localValue;
}

bool RzMetric::hasValueToBeAdded(unsigned long _time, float _value) {
    bool addNewValue = size < 2 || _time - values[size - 2].time >= MAX_DELAY_BETWEEN_MEASURES;
    //Serial.printf("size: %d - deltaTime: %lu max: %d -> result: %i\r\n",size, _time-values[size-2].time, MAX_DELAY_BETWEEN_MEASURES, addNewValue);
    for (int i = 0;
         !addNewValue && i < MAX_VALUES_TO_COMPARE && i < size; i++) { // TODO: utiliser pente de la courbe <??
        int coef = i + 1;
        float diff = fabs(_value - values[size - coef].value);
        float diff_threshold = IGNORED_DIFFERENCE * coef;
        addNewValue |= (diff >= diff_threshold);
        //Serial.printf("value(%i)=%.2f local=%.2f => diff=%.2f / maxDif=%.2f / result=%i \r\n", 
        //    size-coef, values[size-coef].value, _value, diff, diff_threshold, addNewValue);
    }
    return addNewValue;
}

MetricStruct *RzMetric::getValues() { return values; }

int RzMetric::getSize() const { return size; }
