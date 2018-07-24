#include "arduino-defines.h"
#include "gui.h"

void GuiU8G2::Init(uint8_t addr)
{
  LOG("GuiU8G2::Init: address 0x%02x", addr);
}

void GuiU8G2::Powersave(uint8_t mode)
{
  LOG("GuiU8G2::Powersave: mode: %d", mode);
}

void GuiU8G2::ResetToMainScr()
{
  LOG("GuiU8G2::ResetToMainScr: main screen");
}

void GuiU8G2::ShowCDScr()
{
  LOG("GuiU8G2::ShowCDScr: connecting...");
}

void GuiU8G2::ShowCEScr()
{
  LOG("GuiU8G2::ShowCEScr: connected!");
}
