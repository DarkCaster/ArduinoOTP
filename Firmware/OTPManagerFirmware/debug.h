#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

#ifdef DEBUG
void LOG(const __FlashStringHelper* message, const bool nl=true);
void LOG(const int32_t intNumber, const bool nl=true);
void LOG(const uint32_t intNumber, const bool nl=true);
#else
#define LOG(...) ({})
#endif

#endif
