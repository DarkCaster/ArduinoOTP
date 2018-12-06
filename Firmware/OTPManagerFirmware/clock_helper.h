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
		virtual bool SetTime(const uint8_t &sec, const uint8_t &min, const uint8_t &hour, const uint8_t &day, const uint8_t &dow, const uint8_t &month, const uint16_t &year, const uint32_t &utcOffset) = 0;
		virtual uint32_t GetSeed() = 0;
};

#endif
