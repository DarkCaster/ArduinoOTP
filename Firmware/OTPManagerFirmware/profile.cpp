#include "profile.h"


Profile Profile::Invalid()
{
	Profile result;
	result.name[0]='\0';
	result.type= ProfileType::Invalid;
	return result;
}

Profile Profile::Empty()
{
	Profile result;
	result.name[0]='\0';
	result.type= ProfileType::Empty;
	return result;
}

