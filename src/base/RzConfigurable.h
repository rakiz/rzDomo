#ifndef _RZ_CONFIGURABLE_H_
#define _RZ_CONFIGURABLE_H_

#include <Arduino.h>
#include "base/RzIdentifiable.h"
#include "tools/FilesTools.h"

using timeMs = ulong;

class RzConfigurable : public RzIdentifiable {
public:
    /**
     * Get the component configuration (as JSON format)
     * BEWARE to cleanup the memory afterwards !!
     * @return const char*
     */
    virtual const char *getJsonConfig() = 0;

    virtual void loadConfiguration() = 0;

    virtual void saveConfiguration() = 0;

    File getConfigurationFile(const char *prefix, bool isRead);

};

#endif
