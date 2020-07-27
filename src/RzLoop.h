#ifndef _RZ_LOOP_H_
#define _RZ_LOOP_H_


class RzLoop {
public:
    virtual void loop(unsigned long _currentMillis) = 0;
};

#endif