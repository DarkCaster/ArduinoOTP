#ifndef ARDUINO_DEFINES
#define ARDUINO_DEFINES

#include <cstdint>

#define PROGMEM
#define pgm_read_byte(address) (*(address))

unsigned long millis();
void LOG(const char * message, ... );

#endif
