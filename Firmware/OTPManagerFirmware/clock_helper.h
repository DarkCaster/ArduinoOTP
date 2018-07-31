#ifndef CLOCK_HELPER_H
#define CLOCK_HELPER_H

#include <Arduino.h>
#include "low_power_device.h"

class ClockHelperBase : LowPowerDevice
{
  public:
    //TODO: getting time ticks for various OTP algorithms
    virtual void WriteTimeString(char * const target, const uint8_t maxLen) = 0;
    virtual void WriteDateString(char * const target, const uint8_t maxLen) = 0;
    virtual void Update() = 0;
};

#endif
