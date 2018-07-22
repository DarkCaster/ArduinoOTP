#ifndef ARDUINO_DEFINES
#define ARDUINO_DEFINES

#include <chrono>

#define PROGMEM
#define pgm_read_byte(address) (*(address))
#define millis() ((unsigned long)(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())).count())
void LOG(const char * message, ... );

#endif
