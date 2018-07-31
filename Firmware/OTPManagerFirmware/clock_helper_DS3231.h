#ifndef CLOCK_HELPER_DS3231_H
#define CLOCK_HELPER_DS3231_H

#include <Arduino.h>
#include <DS3231_Simple.h>
#include "clock_helper.h"

class ClockHelperDS3231 final : ClockHelperBase
{
  private:
    const uint8_t rtcPowerPin;
    DS3231_Simple clock;
    DateTime lastTime;
  public:
    ClockHelperDS3231(uint8_t rtcPowerPin);
    //TODO: getting time ticks for various OTP algorithms
    void Init() final;
    void WriteTimeString(char * const target, const uint8_t maxLen) final;
    void WriteDateString(char * const target, const uint8_t maxLen) final;
    void WakeupPre() final;
    void DescendPre() final;
    void WakeupPost() final;
    void DescendPost() final;
    void Update() final;
};

#endif
