#ifndef PROFILES_RING_BUFFER_H
#define PROFILES_RING_BUFFER_H

#include <Arduino.h>
#include "profile.h"

struct ProfileItem
{
	private:
		ProfileItem * nextItem;
	public:
		Profile profile;
		uint16_t index;
		ProfileItem * Next() { return nextItem; }
		ProfileItem() = default;
		ProfileItem(Profile &profile, uint16_t index, ProfileItem * nextItem) :
		  nextItem(nextItem),
		  profile(profile),
		  index(index)
		{}
		void Set(Profile &profile, uint16_t index)
		{
			this->profile=profile;
			this->index=index;
		}
};

template<size_t PSZ>
class ProfilesRingBuffer
{
	private:
		ProfileItem items[PSZ];
		ProfileItem * head;
		ProfileItem * tail;
	public:
		ProfileItem * GetHead() { return head; }
		ProfileItem * GetTail() { return tail; }
		void Shift(uint16_t count=1)
		{
			for(uint16_t i=0; i<count; ++i)
			{
				tail=head;
				head=head->Next();
				auto emptyProfile=Profile::Empty();
				tail->Set(emptyProfile,0);
			}
		}
		void Clear()
		{
			auto emptyProfile=Profile::Empty();
			for(size_t i=0; i<PSZ; ++i)
				(items+i)->Set(emptyProfile,0);
		}
		ProfilesRingBuffer()
		{
			auto emptyProfile=Profile::Empty();
			for(size_t i=0; i<PSZ; ++i)
				if(i==PSZ-1)
					*(items+i)=ProfileItem(emptyProfile,0,items+0);
				else
					*(items+i)=ProfileItem(emptyProfile,0,items+i+1);
			head=items+0;
			tail=items+PSZ-1;
		}
};

#endif
