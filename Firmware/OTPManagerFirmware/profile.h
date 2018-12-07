#ifndef PROFILE_H
#define PROFILE_H

#include <Arduino.h>
#include "configuration.h"
#include "helper_defines.h"

#pragma pack(push, profile, 1)
enum class ProfileType : uint8_t
{
	Invalid = 0_u8,
	Empty = 1_u8,
};

struct Profile
{
	ProfileType type;
	char name[PROFILE_NAME_LEN];
};
#pragma pack(pop, profile)

#endif
