#include "configuration.h"
#include "clock_helper_DS3231.h"
#include <DS3231_Simple.h>

static const char daysOfTheWeek[MAIN_SCREEN_DOW_LEN][MAIN_SCREEN_DAY_LEN] PROGMEM = MAIN_SCREEN_DOW;

static void WritePositiveNumber(char * const target, uint8_t &pos, const uint16_t number, const uint8_t zeros, const uint8_t signs)
{
   for(uint8_t i=0;i<zeros;++i)
     *(target+pos++)='0';
   itoa(number,(target+pos),10);
   pos+=signs-zeros;
}

static void Write2SignNumber(char * const target, uint8_t &pos, const uint8_t number)
{
  uint8_t zeros=number>9?0:1;
  WritePositiveNumber(target, pos, number, zeros, 2);
}

/*static void Write4SignNumber(char * const target, uint8_t &pos, const uint16_t number)
{
  uint8_t zeros=number>999?0:number>99?1:number>9?2:3;
  WritePositiveNumber(target, pos, number, zeros, 4);
}*/

void ClockHelperDS3231::WriteTimeString(char * const target, const uint8_t maxLen)
{
  if(maxLen<6)
  {
    *(target)=0;
    return;
  }
  uint8_t pos=0;
  //write hour
  Write2SignNumber(target,pos,lastTime.Hour);
  //write delim
  *(target+pos++)=':';
  //write hour
  Write2SignNumber(target,pos,lastTime.Minute);
  *(target+pos++)=0;
}

void ClockHelperDS3231::WriteDateString(char * const target, const uint8_t maxLen)
{
  if(maxLen<32)
  {
    *(target)=0;
    return;
  }
  uint8_t pos=0;
  //write day of the week
  for(uint8_t i=0;i<MAIN_SCREEN_DAY_LEN-1;++i)
    *(target+pos++)=static_cast<char>(pgm_read_byte(*(daysOfTheWeek+lastTime.Dow)+i));
  *(target+pos++)=' ';
  Write2SignNumber(target,pos,lastTime.Day);
  *(target+pos++)='/';
  Write2SignNumber(target,pos,lastTime.Month);
  *(target+pos++)='/';
  Write2SignNumber(target,pos,lastTime.Year);
  *(target+pos++)=0;
}

ClockHelperDS3231::ClockHelperDS3231(uint8_t _rtcPowerPin, SettingsManager &_settingsManager) :
  rtcPowerPin(_rtcPowerPin),
  settingsManager(_settingsManager),
  lastTime(DateTime())
{ }

void ClockHelperDS3231::InitPre()
{
  //power-on
  pinMode(rtcPowerPin, OUTPUT);
  digitalWrite(rtcPowerPin,HIGH);
}

void ClockHelperDS3231::InitPost()
{
  clock.begin();
}

void ClockHelperDS3231::WakeupPre()
{
  digitalWrite(rtcPowerPin,HIGH);
}

void ClockHelperDS3231::WakeupPost()
{
  clock.begin();
}

void ClockHelperDS3231::DescendPre() { }

void ClockHelperDS3231::DescendPost()
{
  digitalWrite(rtcPowerPin,LOW);
}

void ClockHelperDS3231::Update()
{
	lastTime=clock.read();
}

bool ClockHelperDS3231::SetTime(const uint8_t &sec, const uint8_t &min, const uint8_t &hour, const uint8_t &day, const uint8_t &dow, const uint8_t &month, const uint16_t &year, const uint32_t &utcOffset)
{
	DateTime target;
	target.Second=sec;
	target.Minute=min;
	target.Hour=hour;
	target.Day=day;
	target.Dow=dow+1; //day of week for DS3231 starts from 1, not 0
	target.Month=month;
	if(year<YEAR_START || year>YEAR_START+199)
		return false;
	uint8_t yearCorr=static_cast<uint8_t>(year-YEAR_START);
	target.Year=yearCorr;
	if(!clock.write(target))
		return false;
	//save utcOffset to eeprom;
	settingsManager.settings.utcOffset=utcOffset;
	settingsManager.Commit();
	Update();
	return true;
}
