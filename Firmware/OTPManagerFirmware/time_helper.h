#ifndef TIME_HELPER_H
#define TIME_HELPER_H

#include <Arduino.h>

class TimeHelper
{
  public:
    static void WriteTimeString(char * const target, const uint8_t maxLen);
    static void Wakeup();
    static void Update();
};

#endif
