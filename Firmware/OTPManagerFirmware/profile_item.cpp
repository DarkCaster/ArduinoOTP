#include "profile_item.h"

ProfileItem * ProfileItem::Next() { return nextItem; }

ProfileItem::ProfileItem(Profile &profile, uint16_t index, ProfileItem *nextItem) :
  nextItem(nextItem),
  profile(profile),
  index(index)
{}

void ProfileItem::Set(Profile &profile, uint16_t index)
{
	this->profile=profile;
	this->index=index;
}
