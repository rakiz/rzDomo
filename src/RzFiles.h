#ifndef _RZ_FILES_H_
#define _RZ_FILES_H_

#include <Arduino.h>
#include <FS.h>   // Include the SPIFFS library

#include "base/RzComponent.h"

class RzFiles : public RzComponent { // TODO: make it configurable for auto cleanup old metric files
public:
    RzFiles();

    void setup() override;

    void loop(timeMs referenceTime) override;

    const char *getId() override;

    const char *getDisplayName() override;

    const char *getPrefix() override;

    static File openRead(const char *path);

    static File openWrite(const char *path);

private:
};

#endif