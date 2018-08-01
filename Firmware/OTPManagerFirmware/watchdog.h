#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <Arduino.h>

class WatchdogBase
{
  public:
    virtual void Enable(uint16_t maxDelayMS) = 0;
    virtual void Disable() = 0;
    virtual void Ping() = 0;
};

#endif
