#ifndef GUI_H
#define GUI_H

#ifdef STANDALONE_APP
#include "arduino-defines.h"
#else
#include <Arduino.h>
#endif

enum class MenuItemType : uint8_t
{
    MainScreen,
    MainMenu,
    ProfileMenu,
    ProfileItem,
    SettingsMenu,
    SettingsItem,
};

struct MenuItem
{
    MenuItemType itemType;
    int16_t itemIndex; //<0 - no item was selected
};

class GuiU8G2
{
  public:
    void Init(uint8_t addr=0x78);
    void Powersave(uint8_t mode);
    void ResetToMainScr();
    void ShowCDScr();
    void ShowCEScr();
    void ShowCodeScr(const char * const code);

    //menu navigation methods, show, navigate and select menu items

    //show menu, move to the next menu item
    void MenuNext();
    //go to the parent submenu
    void MenuUp();
    //advance submenu, select menu item. Return item description that was selected
    MenuItem MenuSelect();

    //TODO: reload and fillup menu
};

#endif
