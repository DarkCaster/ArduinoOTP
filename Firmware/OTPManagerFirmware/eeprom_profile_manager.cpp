#include "eeprom_profile_manager.h"
#include "eeprom_reader.h"
#include "eeprom_writer.h"
#include "lcg_random.h"
#include "configuration.h"
#include "debug.h"

EEPROMProfileManager::EEPROMProfileManager(const int baseAddr, const int maxLen, Cipher &cipher, const uint8_t * const key, const uint8_t * const tweak, const size_t KSZ, const size_t TSZ) :
  key(key),
  tweak(tweak),
  keySz(cipher.GetKeySize()),
  tweakSz(cipher.GetTweakSize()),
  baseAddr(baseAddr),
  addrLimit(baseAddr+maxLen),
  profileHdrSz(EEPROMReader::CalculateTotalBlocksCount(sizeof(Profile),cipher.GetBlockSize())),
  profileFullSz(profileHdrSz+EEPROMReader::CalculateTotalBlocksCount(PROFILE_PAYLOAD_LEN,cipher.GetBlockSize())),
  cipher(cipher)
{
	if(keySz>KSZ)
		FAIL(100,100);
	if(tweakSz>TSZ)
		FAIL(100,500);
}

uint16_t EEPROMProfileManager::GetProfilesCount()
{
	if(sizeof(Profile)==0)
		return 0;
	return EEPROM_PROFILE_STORAGE_LEN/profileFullSz;
}

uint16_t EEPROMProfileManager::GetProfileDataSize()
{
	return PROFILE_PAYLOAD_LEN;
}

static void PrepareTmpTweak(uint16_t index, uint16_t extraSeed, uint8_t * const tmpTweak, const uint8_t * const tweak, const size_t tweakSz)
{
	//modify tweak array, according to index
	LCGRandom random(index+extraSeed);
	uint8_t tmpTweakMod[tweakSz];
	random.NextBytes(tmpTweakMod,tweakSz);
	for(size_t i=0;i<tweakSz;++i)
		*(tmpTweak+i)=*(tweak)^*(tmpTweakMod+i);
}

Profile EEPROMProfileManager::ReadProfileHeader(uint16_t index)
{
	//get base address
	const auto profileAddr=static_cast<uint16_t>(baseAddr)+profileFullSz*index;
	if(static_cast<int>(profileAddr+profileFullSz)>addrLimit)
		return Profile::Invalid();
	//create EEPROMReader
	uint8_t tmpTweak[tweakSz];
	PrepareTmpTweak(index,0,tmpTweak,tweak,tweakSz);
	EEPROMReader reader(static_cast<int>(profileAddr),static_cast<int>(profileHdrSz),cipher,key,tmpTweak);
	//profile struct
	Profile result;
	auto ec = reader.ReadData(reinterpret_cast<uint8_t*>(&result),sizeof(Profile));
	if(ec<0)
		FAIL(500,5000);
	if(ec==0)
		return Profile::Invalid();
	return result;
}

bool EEPROMProfileManager::ReadProfileData(uint16_t index, uint8_t* const data)
{
	//get base address
	const auto profileAddr=static_cast<uint16_t>(baseAddr)+profileFullSz*index;
	if(static_cast<int>(profileAddr+profileFullSz)>addrLimit)
		return false;
	const auto profileDataAddr=profileAddr+profileHdrSz;
	//create EEPROMReader
	uint8_t tmpTweak[tweakSz];
	PrepareTmpTweak(index,UINT16_MAX,tmpTweak,tweak,tweakSz);
	EEPROMReader reader(static_cast<int>(profileDataAddr),static_cast<int>(profileFullSz-profileHdrSz),cipher,key,tmpTweak);
	auto ec = reader.ReadData(data,PROFILE_PAYLOAD_LEN);
	if(ec<0)
		FAIL(500,5000);
	if(ec==0)
		return false;
	return true;
}

bool EEPROMProfileManager::WriteProfile(uint16_t index, const Profile& profile, const uint8_t* const data)
{
	//get base address
	const auto profileAddr=static_cast<uint16_t>(baseAddr)+profileFullSz*index;
	if(static_cast<int>(profileAddr+profileFullSz)>addrLimit)
		return false;
	const auto profileDataAddr=profileAddr+profileHdrSz;
	//create EEPROMWriter
	uint8_t tmpTweak[tweakSz];
	PrepareTmpTweak(index,0,tmpTweak,tweak,tweakSz);
	EEPROMWriter writer(static_cast<int>(profileAddr),static_cast<int>(profileHdrSz),cipher,key,tmpTweak);
	if(!writer.WriteData(reinterpret_cast<const uint8_t*>(&profile),sizeof(Profile)))
		FAIL(1000,5000);
	//create EEPROMWriter
	PrepareTmpTweak(index,UINT16_MAX,tmpTweak,tweak,tweakSz);
	EEPROMWriter dataWriter(static_cast<int>(profileDataAddr),static_cast<int>(profileFullSz-profileHdrSz),cipher,key,tmpTweak);
	if(!dataWriter.WriteData(data,PROFILE_PAYLOAD_LEN))
		FAIL(1000,5000);
	return true;
}
