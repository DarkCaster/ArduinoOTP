#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include "configuration.h"

#pragma pack(push, settings, 1)
struct Settings
{
		int32_t utcOffset;
		uint8_t  encTweak[ENCRYPTION_TWEAK_LEN];
};
#pragma pack(pop, settings)

#endif
