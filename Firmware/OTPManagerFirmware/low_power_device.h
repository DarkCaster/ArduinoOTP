#ifndef LOW_POWER_DEVICE_H
#define LOW_POWER_DEVICE_H

#include <Arduino.h>


class LowPowerDevice
{
  public:
    virtual void InitPre() = 0;
    virtual void InitPost() = 0;
    virtual void DescendPre() = 0;
    virtual void DescendPost() = 0;
    virtual void WakeupPre() = 0;
    virtual void WakeupPost() = 0;
};

#endif
