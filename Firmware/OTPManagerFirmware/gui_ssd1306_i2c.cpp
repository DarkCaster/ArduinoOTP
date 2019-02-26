#include <U8g2lib.h>
#include "configuration.h"
#include "gui_ssd1306_i2c.h"
#include "debug.h"

static U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

GuiSSD1306_I2C::GuiSSD1306_I2C(uint8_t displayPowerPin, uint8_t displayAddr, ClockHelper &clockHelper, ProfileManager &profileManager, CodeGenAggregator& codeGenAggregator) :
  displayPowerPin(displayPowerPin),
  displayAddr(displayAddr),
  clockHelper(clockHelper),
  profileManager(profileManager),
  codeGenAggregator(codeGenAggregator),
  rnd(0),
  curItem(MenuItemType::MainScreen),
  menuPos(0)
{ }

void GuiSSD1306_I2C::InitPre()
{
	//power-on
	pinMode(displayPowerPin, OUTPUT);
	digitalWrite(displayPowerPin, HIGH);
	curItem=MenuItemType::MainScreen;
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

MenuItemType GuiSSD1306_I2C::GetCurItem()
{
	return curItem;
}

void GuiSSD1306_I2C::DrawCaption(const char * caption)
{
	auto xPos=static_cast<uint8_t>(rnd.Next(CAPTION_MIN_POS_X, CAPTION_MAX_POS_X));
	auto yPos=static_cast<uint8_t>(rnd.Next(CAPTION_MIN_POS_Y, CAPTION_MAX_POS_Y));
	u8g2.firstPage();
	u8g2.setFont(MENU_MAIN_FONT);
	do {
		u8g2.drawStr(xPos,yPos,caption);
	} while ( u8g2.nextPage()!=0 );
}

void GuiSSD1306_I2C::DrawCaption(const __FlashStringHelper* fCaption)
{
	//instead of using fat strlen_P+strcpy_P or strncpy_P use this simplified code
	//saves 72 bytes of progmem if not using strlen_P and strcpy_P methods anywhere else
	char caption[CAPTION_MAX_LEN+1];
	caption[CAPTION_MAX_LEN]='\0';
	for(uint16_t pos=0; pos<CAPTION_MAX_LEN; ++pos)
	{
		char testChar=pgm_read_byte(reinterpret_cast<const char*>(fCaption)+pos);
		*(caption+pos)=testChar;
		if(testChar=='\0')
			break;
	}
	DrawCaption(caption);
}

void GuiSSD1306_I2C::ShowCDScr()
{
	DrawCaption(F("<Resync>"));
}

void GuiSSD1306_I2C::ShowCEScr()
{
	DrawCaption(F("<Connected>"));
}

void GuiSSD1306_I2C::ShowCodeScr(const char * const code)
{
	DrawCaption(code);
}

void GuiSSD1306_I2C::DrawProfileMenu()
{
	u8g2.firstPage();
	u8g2.setFont(MENU_MAIN_FONT);
	do
	{
		auto item=prBuffer.GetHead();
		for(uint8_t curPos=0; curPos<PROFILES_MENU_ITEMS_COUNT; ++curPos)
		{
			if(item->profile.type==ProfileType::Empty)
				break;
			if(curPos==menuPos)
				u8g2.drawStr(0,MENU_CAPTION_HEIGHT*curPos,">");
			u8g2.drawStr(16,MENU_CAPTION_HEIGHT*curPos,item->profile.name);
			item=item->Next();
		}
	} while ( u8g2.nextPage()!=0 );
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
			head->Set(profile,prIdx);
			head=head->Next();
		}
		if(head==prBuffer.GetTail())
			break;
	}
	//set cutItem for external queries
	curItem=MenuItemType::ProfileMenu;
	STATUS();
	LOG(F("MenuReset"));
}

void GuiSSD1306_I2C::Reseed()
{
	rnd=LCGRandom(clockHelper.GetSeed());
}

void GuiSSD1306_I2C::MenuNext()
{
	if(curItem==MenuItemType::MainScreen)
	{
		DrawCaption(F("<Loading>"));
		STATUS();
		LOG(F("Menu init from MainScreen"));
		MenuReset();
		//if prBuffer is empty, goto main menu
		if(prBuffer.GetHead()->profile.type==ProfileType::Empty)
		{
			ResetToMainScr();
			return;
		}
		DrawProfileMenu();
		return;
	}

	if(curItem==MenuItemType::ProfileMenu)
	{
		STATUS();
		LOG(F("Select next profile menu item"));
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
				tail->Set(profile,prIdx);
				//decrease menuPos
				--menuPos;
				//and select new buffer's item
				bItem=prBuffer.GetHead();
				for(uint8_t testPos=0; testPos<menuPos; ++testPos)
					bItem=bItem->Next();
			}
		}
		//set curItem
		curItem=MenuItemType::ProfileMenu;
		DrawProfileMenu();
		return;
	}

	if(curItem==MenuItemType::ProfileItem)
	{
		STATUS();
		LOG(F("Go back to profile menu"));
		//go back to ProfileMenu
		curItem=MenuItemType::ProfileMenu;
		DrawProfileMenu();
		return;
	}
}

void GuiSSD1306_I2C::MenuSelect()
{
	if(curItem==MenuItemType::MainScreen)
		return;	//do nothing

	if(curItem==MenuItemType::ProfileMenu || curItem==MenuItemType::ProfileItem)
	{
		STATUS();
		LOG(F("Selecting profile item"));
		//get profile from prBuffer, using menuPos
		auto bItem=prBuffer.GetHead();
		for(uint8_t testPos=0; testPos<menuPos; ++testPos)
			bItem=bItem->Next();
		//load profile data
		uint8_t profilePayload[PROFILE_PAYLOAD_LEN];
		profileManager.ReadProfileData(bItem->index,profilePayload);
		//get codegen manager from aggregator
		auto codeGenManager=codeGenAggregator.GetManager(bItem->profile.type);
		if(codeGenManager==nullptr)
		{
			DrawCaption(F("PROFILE\nUNSUPPORTED"));
			return;
		}
		//generate and display code using codegen manager and profilePayload
		char caption[CAPTION_MAX_LEN+1];
		caption[CAPTION_MAX_LEN]='\0';
		bool updateNeeded=codeGenManager->GenerateCode(caption, profilePayload);
		DrawCaption(caption);
		//update profilePayload data and save it
		if(updateNeeded)
			profileManager.WriteProfileData(bItem->index,profilePayload);
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
	} while ( u8g2.nextPage()!=0 );
	curItem=MenuItemType::MainScreen;
	STATUS();
	LOG(F("ResetToMainScr"));
}



