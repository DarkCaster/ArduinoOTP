#ifndef WATCHDOG_AVR_H
#define WATCHDOG_AVR_H

#include <Arduino.h>
#include "watchdog.h"

class WatchdogAVR final : WatchdogBase
{
  public:
    void Enable(uint16_t maxDelayMS) final;
    void Disable() final;
    void Ping() final;
};

#endif
