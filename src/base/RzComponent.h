#ifndef _RZ_COMPONENT_H_
#define _RZ_COMPONENT_H_

#include <Arduino.h>
#include "base/RzIdentifiable.h"

using timeMs = ulong;

class RzComponent : public RzIdentifiable {
public:

    virtual void setup() = 0;

    virtual void loop(timeMs referenceTime) = 0;

};

#endif