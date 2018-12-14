#ifndef PROFILE_ITEM_H
#define PROFILE_ITEM_H

#include <Arduino.h>
#include "profile.h"

struct ProfileItem
{
	private:
		ProfileItem * nextItem;
	public:
		Profile profile;
		uint16_t index;
		ProfileItem * Next();
		void Set(Profile &profile, uint16_t index);
		ProfileItem() = default;
		ProfileItem(Profile &profile, uint16_t index, ProfileItem * nextItem);
};

#endif
