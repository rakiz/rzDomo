#include "base/RzConfigurable.h"

File RzConfigurable::getConfigurationFile(const char *prefix, bool isRead) {
    char path[40];

    int ret = snprintf(path, sizeof path, "%s-%s.conf", prefix, getId());
    if (ret < 0 || ret >= (int) sizeof path) {
        Serial.println(F("Error while generating configuration filename"));
        return File();
    }

    const File &file = isRead ? openRead(path) : openWrite(path);
    //    Serial.printf("Configuration file %s %s\r\n", path, file?"found, loading.":"not found, using default.");
    return file;
}