#ifndef CLOCK_HELPER_DS3231_H
#define CLOCK_HELPER_DS3231_H

#include <Arduino.h>
#include <DS3231_Simple.h>
#include "clock_helper.h"
#include "settings_manager.h"

class ClockHelperDS3231 final : public ClockHelperBase
{
  private:
    const uint8_t rtcPowerPin;
		SettingsManager &settingsManager;
		DS3231_Simple clock;
    DateTime lastTime;
  public:
		ClockHelperDS3231(uint8_t rtcPowerPin, SettingsManager &settingsManager);
		ClockHelperDS3231(uint8_t rtcPowerPin, SettingsManager &&) = delete;
    //TODO: getting time ticks for various OTP algorithms
    void InitPre() final;
    void InitPost() final;
    void WriteTimeString(char * const target, const uint8_t maxLen) final;
    void WriteDateString(char * const target, const uint8_t maxLen) final;
    void WakeupPre() final;
    void DescendPre() final;
    void WakeupPost() final;
    void DescendPost() final;
    void Update() final;
		bool SetTime(const uint8_t &sec, const uint8_t &min, const uint8_t &hour, const uint8_t &day, const uint8_t &dow, const uint8_t &month, const uint16_t &year, const uint32_t &utcOffset) final;
};

#endif
