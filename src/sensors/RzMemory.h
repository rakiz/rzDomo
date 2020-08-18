#ifndef _RZ_MEMORY_H_
#define _RZ_MEMORY_H_

#include "base/RzSensor.h"
#include "tools/MultiSampling.h"

class RzMemory : public RzSensor {
public:
    RzMemory();

    void setup();

    void loop(timeMs referenceTime);

    const char *getDisplayName() override;

    ~RzMemory() override;

    void loadConfiguration() override;

    void saveConfiguration() override;

    String getJsonConfig() override;

    const char *getPrefix() override;

private:
    // Multisampler configuration
    MultiSampling *_multisampling;


};


#endif
