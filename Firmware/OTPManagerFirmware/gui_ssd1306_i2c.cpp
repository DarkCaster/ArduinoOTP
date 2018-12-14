#include <U8g2lib.h>
#include "configuration.h"
#include "gui_ssd1306_i2c.h"

static U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

GuiSSD1306_I2C::GuiSSD1306_I2C(uint8_t displayPowerPin, uint8_t displayAddr, ClockHelperBase &clockHelper, ProfileManager &profileManager) :
  displayPowerPin(displayPowerPin),
  displayAddr(displayAddr),
  clockHelper(clockHelper),
  profileManager(profileManager),
  rnd(0),
  curItem(MenuItemType::MainScreen,0),
  menuPos(0)
{ }

void GuiSSD1306_I2C::InitPre()
{
  //power-on
  pinMode(displayPowerPin, OUTPUT);
  digitalWrite(displayPowerPin, HIGH);
	curItem=MenuItem(MenuItemType::MainScreen,0);
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

MenuItem GuiSSD1306_I2C::GetCurItem()
{
	return curItem;
}

static void DrawCaption(const char * caption)
{
	u8g2.firstPage();
	u8g2.setFont(MAIN_SCREEN_DATE_FONT);
	do {
		u8g2.drawStr(0,24,caption);
	} while ( u8g2.nextPage() );
}

static void DrawCaption(const __FlashStringHelper* fCaption)
{
	char caption[strlen_P(reinterpret_cast<const char*>(fCaption))];
	strcpy_P(caption,reinterpret_cast<const char*>(fCaption));
	DrawCaption(caption);
}

void GuiSSD1306_I2C::ShowCDScr()
{
	DrawCaption(F("< Resync >"));
}

void GuiSSD1306_I2C::ShowCEScr()
{
	DrawCaption(F("< Connected >"));
}

void GuiSSD1306_I2C::ShowCodeScr(const char * const code)
{
	u8g2.firstPage();
  u8g2.setFont(MAIN_SCREEN_DATE_FONT);
  do {
    u8g2.drawStr(0,24,code);
	} while ( u8g2.nextPage() );
}

void GuiSSD1306_I2C::MenuReset()
{
	prBuffer.Clear();
	menuPos=0;
	auto prLimit=profileManager.GetProfilesCount();
	auto prIdx=prLimit;
	auto head=prBuffer.GetHead();
	for(prIdx=0; prIdx<prLimit; ++prIdx)
	{
		//try to read profile header
		auto profile=profileManager.ReadProfileHeader(prIdx);
		//add it to prBuffer, switch head
		if(profile.type!=ProfileType::Empty && profile.type!=ProfileType::Invalid)
		{
			head->profile=profile;
			head->index=prIdx;
			head=head->Next();
		}
		if(head==prBuffer.GetTail())
			break;
	}
	//set cutItem for external queries
	curItem=MenuItem(MenuItemType::ProfileMenu,prBuffer.GetHead()->index);
}

void GuiSSD1306_I2C::Reseed()
{
	rnd=LCGRandom(clockHelper.GetSeed());
}

void GuiSSD1306_I2C::MenuNext()
{
	if(curItem.itemType==MenuItemType::MainScreen)
	{
		MenuReset();
		//if prBuffer is empty, goto main menu
		if(prBuffer.GetHead()->profile.type==ProfileType::Empty)
		{
			ResetToMainScr();
			return;
		}
		//TODO: visualize menu
		return;
	}

	if(curItem.itemType==MenuItemType::ProfileMenu)
	{
		//increase menuPos
		++menuPos;
		if(menuPos==PROFILES_MENU_ITEMS_COUNT)
		{
			MenuReset();
			return;
		}
		//check, whether we at the empty item now, set menu-pos to zero if so
		auto bItem=prBuffer.GetHead();
		for(uint8_t testPos=0; testPos<menuPos; ++testPos)
			bItem=bItem->Next();
		if(bItem->profile.type==ProfileType::Empty)
		{
			menuPos=0;
			bItem=prBuffer.GetHead();
		}
		//if we switched to the last position, try to read more profiles
		if(menuPos==PROFILES_MENU_ITEMS_COUNT-1)
		{
			//try to read one extra profile from eeprom
			auto prLimit=profileManager.GetProfilesCount();
			auto prIdx=prLimit;
			auto profile=Profile::Empty();
			for(prIdx=bItem->index+1; prIdx<prLimit; ++prIdx)
			{
				profile=profileManager.ReadProfileHeader(prIdx);
				if(profile.type!=ProfileType::Empty && profile.type!=ProfileType::Invalid)
					break;
			}
			//if succeed
			if(prIdx<prLimit)
			{
				//shift profiles' ring buffer
				prBuffer.Shift();
				//and add new profile to the new tail of the buffer
				auto tail=prBuffer.GetTail();
				tail->index=prIdx;
				tail->profile=profile;
				//decrease menuPos
				--menuPos;
				//and select new buffer's item
				bItem=prBuffer.GetHead();
				for(uint8_t testPos=0; testPos<menuPos; ++testPos)
					  bItem=bItem->Next();
			}
		}
		//set curItem
		curItem=MenuItem(MenuItemType::ProfileMenu,bItem->index);
		//TODO: visualize menu
		return;
	}

	if(curItem.itemType==MenuItemType::ProfileItem)
	{
		//go back to ProfileMenu
		curItem=MenuItem(MenuItemType::ProfileMenu,curItem.itemIndex);
		//TODO: visualize menu
		return;
	}
}

void GuiSSD1306_I2C::MenuSelect()
{
	if(curItem.itemType==MenuItemType::MainScreen)
		return;	//do nothing

	if(curItem.itemType==MenuItemType::ProfileMenu || curItem.itemType==MenuItemType::ProfileItem)
	{
		//select current item, generate and display code
		return;
	}

}

void GuiSSD1306_I2C::ResetToMainScr()
{
  char timeString[6];
  char dateString[48];
	clockHelper.Update();
	clockHelper.WriteTimeString(timeString,6);
	clockHelper.WriteDateString(dateString,48);
    auto timeXPos=static_cast<uint8_t>(rnd.Next(MAIN_SCREEN_TIME_MIN_POS_X, MAIN_SCREEN_TIME_MAX_POS_X));
    auto timeYPos=static_cast<uint8_t>(rnd.Next(MAIN_SCREEN_TIME_MIN_POS_Y, MAIN_SCREEN_TIME_MAX_POS_Y));
    auto dateXPos=static_cast<uint8_t>(rnd.Next(MAIN_SCREEN_DATE_MIN_POS_X, MAIN_SCREEN_DATE_MAX_POS_X));
    auto dateYPos=static_cast<uint8_t>(rnd.Next(MAIN_SCREEN_DATE_MIN_POS_Y, MAIN_SCREEN_DATE_MAX_POS_Y));
  u8g2.firstPage();
  do {
    u8g2.setFont(MAIN_SCREEN_DATE_FONT);
    u8g2.drawStr(dateXPos,dateYPos,dateString);
    u8g2.setFont(MAIN_SCREEN_TIME_FONT);
    u8g2.drawStr(timeXPos,timeYPos,timeString);
  } while ( u8g2.nextPage() );
	curItem=MenuItem(MenuItemType::MainScreen,0);
}



