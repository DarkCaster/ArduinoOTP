#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

#ifdef DEBUG
void LOG(const char * message, ... );
#else
#define LOG(...) ({})
#endif

#endif
