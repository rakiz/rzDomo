#ifndef _RZ_CONFIGURABLE_H
#define _RZ_CONFIGURABLE_H

#include <Arduino.h>
#include "base/RzComponent.h"
#include "base/RzIdentifiable.h"
#include "RzFiles.h"

class RzConfigurable : public RzIdentifiable {
public:
    virtual String getJsonConfig() = 0;

    virtual void loadConfiguration() = 0;

    virtual void saveConfiguration() = 0;

    File getConfigurationFile(const char *prefix, bool isRead);

};

#endif
