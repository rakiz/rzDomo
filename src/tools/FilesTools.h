#ifndef _FILES_TOOLS_H_
#define _FILES_TOOLS_H_

#include <Arduino.h>
#include <FS.h>   // Include the SPIFFS library

File openRead(const char *path);

File openWrite(const char *path);


#endif
