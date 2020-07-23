#ifndef _MYFILES_H_
#define _MYFILES_H_

#include <Arduino.h>
#include <FS.h>   // Include the SPIFFS library
#include "RzLoop.h"

class RzFiles : public RzLoop {
public:
    void setup();

    void loop(unsigned long _currentMillis);

    File fileRead(String _path);

private:
};

#endif