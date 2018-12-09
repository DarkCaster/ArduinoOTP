#ifndef GUI_H
#define GUI_H

#include <Arduino.h>
#include "low_power_device.h"

enum class MenuItemType : uint8_t
{
    MainScreen,
	  //MainMenu,
    ProfileMenu,
    ProfileItem,
	  //SettingsMenu,
	  //SettingsItem,
};

struct MenuItem
{
    MenuItem(const MenuItemType itemType, const int16_t itemIndex)
    {
      this->itemType=itemType;
      this->itemIndex=itemIndex;
    }
    MenuItemType itemType;
    int16_t itemIndex;
};

class GuiBase : LowPowerDevice
{
    virtual MenuItem ResetToMainScr() = 0;
    virtual void ShowCDScr() = 0;
    virtual void ShowCEScr() = 0;
    virtual void ShowCodeScr(const char * const code) = 0;

    //menu navigation methods, show, navigate and select menu items

		//called when "Next" button pressed
    virtual void MenuNext() = 0;
		//advance submenu, select menu item. Return item description that was selected. Called when "Select" button pressed
    virtual MenuItem MenuSelect() = 0;
};


#endif
