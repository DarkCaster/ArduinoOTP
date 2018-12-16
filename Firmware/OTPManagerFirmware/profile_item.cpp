#include "profile_item.h"

ProfileItem * ProfileItem::Next() { return nextItem; }

ProfileItem::ProfileItem(Profile &profile, uint16_t index, ProfileItem *nextItem) :
  index(index),
  profile(profile),
  nextItem(nextItem)
{}

void ProfileItem::Set(Profile &profile, uint16_t index)
{
	this->profile=profile;
	this->index=index;
}
