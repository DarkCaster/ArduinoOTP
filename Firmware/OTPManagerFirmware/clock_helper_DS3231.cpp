#include "configuration.h"
#include "clock_helper_DS3231.h"
#include <DS3231_Simple.h>

ClockHelperDS3231::ClockHelperDS3231(uint8_t _rtcPowerPin) : rtcPowerPin(_rtcPowerPin), lastTime(DateTime()) { }

static const char daysOfTheWeek[7][MAIN_SCREEN_DAY_LEN] PROGMEM = MAIN_SCREEN_DAYS_OF_THE_WEEK;

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

static void Write4SignNumber(char * const target, uint8_t &pos, const uint16_t number)
{
  uint8_t zeros=number>999?0:number>99?1:number>9?2:3;
  WritePositiveNumber(target, pos, number, zeros, 4);
}

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

void ClockHelperDS3231::Init()
{
  //power-on
  pinMode(rtcPowerPin, OUTPUT);
  digitalWrite(rtcPowerPin,HIGH);
  delay(10);
  clock.begin();
}

void ClockHelperDS3231::Wakeup()
{
  digitalWrite(rtcPowerPin,HIGH);
  delay(10);
  clock.begin();
}

void ClockHelperDS3231::Descend()
{
  digitalWrite(rtcPowerPin,LOW);
}

void ClockHelperDS3231::Update()
{
  lastTime=clock.read();
}
