#ifndef _RZ_CONFIGURABLE_H_
#define _RZ_CONFIGURABLE_H_

#include <Arduino.h>
#include "base/RzIdentifiable.h"
#include "tools/FilesTools.h"

using timeMs = ulong;

class RzConfigurable : public RzIdentifiable {
public:
    virtual String getJsonConfig() = 0;

    virtual void loadConfiguration() = 0;

    virtual void saveConfiguration() = 0;

    File getConfigurationFile(const char *prefix, bool isRead);

};

#endif
