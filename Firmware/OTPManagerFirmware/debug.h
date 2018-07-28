#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

#ifdef DEBUG
void LOG(const __FlashStringHelper* message, const bool nl=true);
#else
#define LOG(...) ({})
#endif

#endif
