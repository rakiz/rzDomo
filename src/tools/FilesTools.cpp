#include "FilesTools.h"

File openRead(const char *path) {  // send the right file to the client (if it exists)
    if (SPIFFS.exists(path)) {
        return SPIFFS.open(path, "r"); // Open the file
    } else return File();
}

File openWrite(const char *path) {
    return SPIFFS.open(path, "w");
}
