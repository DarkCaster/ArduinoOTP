#ifndef CRC8_H
#define CRC8_H

#ifdef STANDALONE_APP
#include "arduino-defines.h"
#else
#include <Arduino.h>
#endif

uint8_t CRC8(const uint8_t *source, uint8_t len);

#endif
