#ifndef GUI_SSD1306_H
#define GUI_SSD1306_H

#include <Arduino.h>
#include "gui.h"
#include "clock_helper.h"
#include "lcg_random.h"
#include "profile_manager.h"

class GuiSSD1306_I2C final : GuiBase
{
  private:
    const uint8_t displayPowerPin;
    const uint8_t displayAddr;
		ClockHelperBase &clockHelper;
		ProfileManager &profileManager;
		LCGRandom rnd;

  public:
		GuiSSD1306_I2C(uint8_t displayPowerPin, uint8_t displayAddr, ClockHelperBase &clockHelper, ProfileManager &profileManager);
		GuiSSD1306_I2C(uint8_t displayPowerPin, uint8_t displayAddr, ClockHelperBase &&, ProfileManager &&) = delete;
    void InitPre() final;
    void InitPost() final;
    void DescendPre() final;
    void WakeupPre() final;
    void DescendPost() final;
    void WakeupPost() final;
    MenuItem ResetToMainScr() final;
    void ShowCDScr() final;
    void ShowCEScr() final;
    void ShowCodeScr(const char * const code) final;
		void Reseed();

    //menu navigation methods, show, navigate and select menu items
    void MenuNext() final;
    MenuItem MenuSelect() final;
};

#endif
