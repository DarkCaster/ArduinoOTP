#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include <Arduino.h>
#include "profile.h"

class ProfileManager
{
	public:
		virtual uint16_t GetProfilesCount() = 0;
		virtual uint16_t GetProfileDataSize() = 0;
		virtual Profile ReadProfileHeader(uint16_t index) = 0;
		virtual bool ReadProfileData(uint16_t index, uint8_t * const data) = 0;
		virtual bool WriteProfile(uint16_t index, const Profile &profile, const uint8_t * const data) = 0;
};

#endif
