#include <U8g2lib.h>
#include "configuration.h"
#include "gui_ssd1306_i2c.h"

static U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

GuiSSD1306_I2C::GuiSSD1306_I2C(uint8_t displayPowerPin, uint8_t displayAddr, ClockHelperBase &clockHelper, ProfileManager &profileManager) :
  displayPowerPin(displayPowerPin),
  displayAddr(displayAddr),
  clockHelper(clockHelper),
  profileManager(profileManager),
  rnd(0)
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

static void DrawCaption(const char * caption)
{
	u8g2.firstPage();
	u8g2.setFont(MAIN_SCREEN_DATE_FONT);
	do {
		u8g2.drawStr(0,24,caption);
	} while ( u8g2.nextPage() );
}

//TODO: change to progmem (F("< Resync >"))

/*static void DrawCaption(const __FlashStringHelper* fCaption)
{
	char caption[strlen_P(reinterpret_cast<const char*>(fCaption))];
	strcpy_P(caption,reinterpret_cast<const char*>(fCaption));
	DrawCaption(caption);
}*/

void GuiSSD1306_I2C::ShowCDScr()
{
	DrawCaption("< Resync >");
}

void GuiSSD1306_I2C::ShowCEScr()
{
	DrawCaption("< Connected >");
}

void GuiSSD1306_I2C::ShowCodeScr(const char * const code)
{
	u8g2.firstPage();
  u8g2.setFont(MAIN_SCREEN_DATE_FONT);
  do {
    u8g2.drawStr(0,24,code);
	} while ( u8g2.nextPage() );
}

void GuiSSD1306_I2C::Reseed()
{
	rnd=LCGRandom(clockHelper.GetSeed());
}

void GuiSSD1306_I2C::MenuNext()
{

}

MenuItem GuiSSD1306_I2C::MenuSelect()
{
  //TODO:
	return MenuItem(MenuItemType::MainScreen,0);
}

MenuItem GuiSSD1306_I2C::ResetToMainScr()
{
  char timeString[6];
  char dateString[48];
	clockHelper.Update();
	clockHelper.WriteTimeString(timeString,6);
	clockHelper.WriteDateString(dateString,48);
	uint8_t timeXPos=static_cast<uint8_t>(rnd.Next(MAIN_SCREEN_TIME_MIN_POS_X, MAIN_SCREEN_TIME_MAX_POS_X));
	uint8_t timeYPos=static_cast<uint8_t>(rnd.Next(MAIN_SCREEN_TIME_MIN_POS_Y, MAIN_SCREEN_TIME_MAX_POS_Y));
	uint8_t dateXPos=static_cast<uint8_t>(rnd.Next(MAIN_SCREEN_DATE_MIN_POS_X, MAIN_SCREEN_DATE_MAX_POS_X));
	uint8_t dateYPos=static_cast<uint8_t>(rnd.Next(MAIN_SCREEN_DATE_MIN_POS_Y, MAIN_SCREEN_DATE_MAX_POS_Y));
  u8g2.firstPage();
  do {
    u8g2.setFont(MAIN_SCREEN_DATE_FONT);
    u8g2.drawStr(dateXPos,dateYPos,dateString);
    u8g2.setFont(MAIN_SCREEN_TIME_FONT);
    u8g2.drawStr(timeXPos,timeYPos,timeString);
  } while ( u8g2.nextPage() );
	return MenuItem(MenuItemType::MainScreen,0);
}



