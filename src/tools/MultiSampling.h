#ifndef _MULTISAMPLING_H_
#define _MULTISAMPLING_H_

#include <Arduino.h>
#include <functional>

typedef std::function<float(void)> SamplingHandler;

class MultiSampling {
public:
    MultiSampling(int _nbSampling, int _extrems, unsigned long _delaySampling, unsigned long _delayMeasure);

    float sample(unsigned long _currentMillis, const SamplingHandler& _lambda);

    void displayConfig();

private:
    void clean();

    void add(unsigned long _currentMillis, float _value);

    bool needMore() const;

    bool isReady(unsigned long _currentMillis) const;

    float getFinalValue();

    float *buf;
    int nbSampling;
    int extrems;
    int current;
    unsigned long delaySampling;
    unsigned long delayMeasure;
    unsigned long previousSampling;
    unsigned long previousMeasure;
};

#endif
