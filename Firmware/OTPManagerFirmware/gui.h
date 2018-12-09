#ifndef GUI_H
#define GUI_H

#include <Arduino.h>
#include "low_power_device.h"

enum class MenuItemType : uint8_t
{
    MainScreen,
    MainMenu,
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

    //show menu, move to the next menu item
    virtual void MenuNext() = 0;
    //go to the parent submenu
    virtual void MenuUp() = 0;
    //advance submenu, select menu item. Return item description that was selected
    virtual MenuItem MenuSelect() = 0;
};


#endif
