#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <Arduino.h>

#pragma pack(push, settings, 1)
struct Settings
{
	uint32_t utcOffset;
};
#pragma pack(pop, settings)

class SettingsManager
{
	public:
		Settings settings;
		virtual void Commit() = 0;
		virtual void Init() = 0;
};

#endif
