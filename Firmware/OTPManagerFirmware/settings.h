#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

#pragma pack(push, settings, 1)
struct Settings
{
	uint32_t utcOffset;
};
#pragma pack(pop, settings)

#endif
