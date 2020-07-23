#ifndef _MYWIFI_H_
#define _MYWIFI_H_

#include "../RzLoop.h"

class RzWifi : public RzLoop {
public:
    RzWifi(String _dnsName, String _credentials[], int _size);

    void setup();

    void loop(unsigned long _currentMillis);

private:
    String dnsName;
    String *credentials;
    int size;
};

#endif