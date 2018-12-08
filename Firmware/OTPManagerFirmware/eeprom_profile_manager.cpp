#include "eeprom_profile_manager.h"
#include "eeprom_reader.h"
#include "configuration.h"

uint16_t EEPROMProfileManager::GetProfilesCount()
{
	auto psz_t=sizeof(Profile);
	if(psz_t==0)
		return 0;
	//calculate full profile-header size
	auto cbsz=cipher.GetBlockSize();
	return EEPROM_PROFILE_STORAGE_LEN/(EEPROMReader::CalculateTotalBlocksCount(psz_t,cbsz)+EEPROMReader::CalculateTotalBlocksCount(PROFILE_PAYLOAD_LEN,cbsz));
}

uint16_t EEPROMProfileManager::GetProfileDataSize()
{
	return PROFILE_PAYLOAD_LEN;
}

Profile EEPROMProfileManager::ReadProfileHeader(uint16_t index)
{
	//TODO
	return Profile::Invalid();
}

void EEPROMProfileManager::ReadProfileData(uint16_t index, uint8_t* const data)
{
	//TODO
}
