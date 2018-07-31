#ifndef CLOCK_HELPER_H
#define CLOCK_HELPER_H

#include <Arduino.h>

class ClockHelperBase
{
  public:
    //TODO: getting time ticks for various OTP algorithms
    virtual void Init() = 0;
    virtual void WriteTimeString(char * const target, const uint8_t maxLen) = 0;
    virtual void WriteDateString(char * const target, const uint8_t maxLen) = 0;
    virtual void Wakeup() = 0;
    virtual void Descend() = 0;
    virtual void Update() = 0;
};


#endif
