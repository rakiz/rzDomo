#ifndef _MULTISAMPLING_H_
#define _MULTISAMPLING_H_

#include <Arduino.h>
#include <functional>
#include <base/RzConfigurable.h>

typedef std::function<int(void)> SamplingHandler;

class MultiSampling : public RzConfigurable {
public:
    MultiSampling(const char *id, //
                  int nbSampling, int extremes, ulong delaySampling, ulong delayMeasure);

    virtual ~MultiSampling();

    int sample(timeMs currentMillis, const SamplingHandler &lambda);

    void displayConfig() const;

    const char *getJsonConfig() override;

    void loadConfiguration() override;

    void saveConfiguration() override;

    const char *getId() override;

    const char *getDisplayName() override;

    const char *getPrefix() override;

private:
    void clean();

    void add(timeMs currentMillis, int value);

    bool needMore() const;

    bool isReady(timeMs currentMillis) const;

    int getFinalValue();

    const char *_id;
    int *_buf;
    int _size;
    int _extremes;
    int _current{};
    ulong _delaySampling;
    ulong _delayMeasure;
    ulong _previousSampling{};
    ulong _previousMeasure{};
};

#endif
