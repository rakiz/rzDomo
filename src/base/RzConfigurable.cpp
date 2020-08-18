#include "base/RzConfigurable.h"

File RzConfigurable::getConfigurationFile(const char *prefix, bool isRead) {
    char path[40];
    sprintf(path, "%s-%s.conf", prefix, getId());
    const File &file = isRead ? RzFiles::openRead(path) : RzFiles::openWrite(path);
//    Serial.printf("Configuration file %s %s\r\n", path, file?"found, loading.":"not found, using default.");
    return file;
}