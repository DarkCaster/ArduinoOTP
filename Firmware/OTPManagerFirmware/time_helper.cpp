#include "time_helper.h"
#include <DS3231_Simple.h>

static DS3231_Simple clock;
static DateTime lastTime;

void Write2SignNumber(char * const target, uint8_t &pos, const uint8_t number)
{

  if(number<10)
  {
    *(target+pos++)='0';
    itoa(number,target+pos++,10);
  }
  else
  {
    itoa(number,target,10);
    pos+=2;
  }
}

void TimeHelper::WriteTimeString(char * const target, const uint8_t maxLen)
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
  *(target+pos)=':';
  //write hour
  Write2SignNumber(target,pos,lastTime.Minute);
  *(target+pos)=0;
}

void TimeHelper::Wakeup()
{
  clock.begin();
}

void TimeHelper::Update()
{
  lastTime=clock.read();
}
