#include <U8g2lib.h>
#include "configuration.h"
#include "gui.h"
#include "time_helper.h"

static U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void GuiU8G2::Init(uint8_t addr)
{
  u8g2.setI2CAddress(addr);
  u8g2.begin();
}

//TODO:

void GuiU8G2::ShowCDScr()
{
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_crox2cb_tf);
  do {
    u8g2.drawStr(0,24,"< Resync >");
  } while ( u8g2.nextPage() );
}

void GuiU8G2::ShowCEScr()
{
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_crox2cb_tf);
  do {
    u8g2.drawStr(0,24,"< Connected >");
  } while ( u8g2.nextPage() );
}

void GuiU8G2::Powersave(uint8_t mode)
{
  u8g2.setPowerSave(mode);
}

void GuiU8G2::ResetToMainScr()
{
  char timeString[6];
  TimeHelper::WriteTimeString(timeString,6);
  uint8_t xPos=random(MAIN_SCREEN_TIME_MIN_POS_X, MAIN_SCREEN_TIME_MAX_POS_X);
  uint8_t yPos=random(MAIN_SCREEN_TIME_MIN_POS_Y, MAIN_SCREEN_TIME_MAX_POS_Y);
  u8g2.firstPage();
  u8g2.setFont(MAIN_SCREEN_TIME_FONT);
  do {
    u8g2.drawStr(xPos,yPos,timeString);
  } while ( u8g2.nextPage() );
}
