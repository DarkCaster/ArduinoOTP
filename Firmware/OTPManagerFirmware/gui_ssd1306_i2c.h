#ifndef GUI_SSD1306_H
#define GUI_SSD1306_H

#include <Arduino.h>
#include "gui.h"
#include "clock_helper.h"
#include "lcg_random.h"
#include "profile_manager.h"
#include "profiles_ring_buffer.h"
#include "configuration.h"

class GuiSSD1306_I2C final : Gui
{
	private:
		const uint8_t displayPowerPin;
		const uint8_t displayAddr;
		ClockHelper &clockHelper;
		ProfileManager &profileManager;
		LCGRandom rnd;
		MenuItemType curItem;
		uint8_t menuPos;
		ProfilesRingBuffer<PROFILES_MENU_ITEMS_COUNT> prBuffer;
		void MenuReset();
		void DrawProfileMenu();
		void DrawCaption(const __FlashStringHelper* fCaption);
		void DrawCaption(const char* caption);
	public:
		GuiSSD1306_I2C(uint8_t displayPowerPin, uint8_t displayAddr, ClockHelper &clockHelper, ProfileManager &profileManager);
		GuiSSD1306_I2C(uint8_t displayPowerPin, uint8_t displayAddr, ClockHelper &&, ProfileManager &&) = delete;
		void InitPre() final;
		void InitPost() final;
		void DescendPre() final;
		void WakeupPre() final;
		void DescendPost() final;
		void WakeupPost() final;

		MenuItemType GetCurItem() final;
		void ResetToMainScr() final;
		void ShowCDScr() final;
		void ShowCEScr() final;
		void ShowCodeScr(const char * const code) final;
		void Reseed();

		//menu navigation methods, show, navigate and select menu items
		void MenuNext() final;
		void MenuSelect() final;
};

#endif
