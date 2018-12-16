#ifndef PROFILE_ITEM_H
#define PROFILE_ITEM_H

#include <Arduino.h>
#include "profile.h"

struct ProfileItem
{
	public:
		ProfileItem * Next();
		void Set(Profile &profile, uint16_t index);
		ProfileItem() = default;
		ProfileItem(Profile &profile, uint16_t index, ProfileItem * nextItem);

		uint16_t index;
		Profile profile;
	private:
		ProfileItem * nextItem;
};

#endif
