#include <U8g2lib.h>
#include "configuration.h"
#include "gui_ssd1306_i2c.h"

static U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

GuiSSD1306_I2C::GuiSSD1306_I2C(uint8_t _displayPowerPin, uint8_t _displayAddr, ClockHelperBase * const _clockHelper) :
  displayPowerPin(_displayPowerPin),
  displayAddr(_displayAddr),
  clockHelper(_clockHelper)
{ }

void GuiSSD1306_I2C::InitPre()
{
  //power-on
  pinMode(displayPowerPin, OUTPUT);
  digitalWrite(displayPowerPin, HIGH);
}

void GuiSSD1306_I2C::InitPost()
{
  u8g2.setI2CAddress(displayAddr);
  u8g2.begin();
}

void GuiSSD1306_I2C::DescendPre()
{
  u8g2.clearDisplay();
}

void GuiSSD1306_I2C::DescendPost()
{
  digitalWrite(displayPowerPin,LOW);
}

void GuiSSD1306_I2C::WakeupPre()
{
  digitalWrite(displayPowerPin,HIGH);
}

void GuiSSD1306_I2C::WakeupPost()
{
  u8g2.begin();
}

//TODO:

void GuiSSD1306_I2C::ShowCDScr()
{
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_crox2cb_tf);
  do {
    u8g2.drawStr(0,24,"< Resync >");
  } while ( u8g2.nextPage() );
}

void GuiSSD1306_I2C::ShowCEScr()
{
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_crox2cb_tf);
  do {
    u8g2.drawStr(0,24,"< Connected >");
  } while ( u8g2.nextPage() );
}

void GuiSSD1306_I2C::ShowCodeScr(const char * const code)
{
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_crox2cb_tf);
  do {
    u8g2.drawStr(0,24,code);
  } while ( u8g2.nextPage() );
}

void GuiSSD1306_I2C::MenuNext()
{

}

void GuiSSD1306_I2C::MenuUp()
{

}

MenuItem GuiSSD1306_I2C::MenuSelect()
{
  //TODO:
  return MenuItem(MenuItemType::MainMenu,0);
}

MenuItem GuiSSD1306_I2C::ResetToMainScr()
{
  char timeString[6];
  char dateString[48];
  clockHelper->WriteTimeString(timeString,6);
  clockHelper->WriteDateString(dateString,48);
  uint8_t timeXPos=random(MAIN_SCREEN_TIME_MIN_POS_X, MAIN_SCREEN_TIME_MAX_POS_X);
  uint8_t timeYPos=random(MAIN_SCREEN_TIME_MIN_POS_Y, MAIN_SCREEN_TIME_MAX_POS_Y);
  uint8_t dateXPos=random(MAIN_SCREEN_DATE_MIN_POS_X, MAIN_SCREEN_DATE_MAX_POS_X);
  uint8_t dateYPos=random(MAIN_SCREEN_DATE_MIN_POS_Y, MAIN_SCREEN_DATE_MAX_POS_Y);
  u8g2.firstPage();
  do {
    u8g2.setFont(MAIN_SCREEN_DATE_FONT);
    u8g2.drawStr(dateXPos,dateYPos,dateString);
    u8g2.setFont(MAIN_SCREEN_TIME_FONT);
    u8g2.drawStr(timeXPos,timeYPos,timeString);
  } while ( u8g2.nextPage() );
  return MenuItem(MenuItemType::MainMenu,0);
}



