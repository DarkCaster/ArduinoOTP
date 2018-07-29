#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

#define D_INT(n) ((int32_t)(n))
#define D_UINT(n) ((uint32_t)(n))

#ifdef DEBUG
void LOG(const __FlashStringHelper* message, const bool nl=true, const bool hdr=true);
void LOG(const int32_t intNumber, const bool nl=true, const bool hdr=true);
void LOG(const uint32_t intNumber, const bool nl=true, const bool hdr=true);
#else
#define LOG(...) ({})
#endif

#endif
