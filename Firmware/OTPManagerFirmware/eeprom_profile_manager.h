#ifndef EEPROM_PROFILE_MANAGER_H
#define EEPROM_PROFILE_MANAGER_H

#include <Arduino.h>
#include "profile_manager.h"
#include "cipher.h"

class EEPROMProfileManager final : public ProfileManager
{
	private:
		const uint8_t * const key;
		const uint8_t * const tweak;
		const size_t keySz;
		const size_t tweakSz;
		const int baseAddr;
		const int addrLimit;
		const uint16_t profileHdrSz;
		const uint16_t profileFullSz;
		CipherBase &cipher;
		EEPROMProfileManager(const int baseAddr, const int maxLen, CipherBase &cipher, const uint8_t * const key, const uint8_t * const tweak, const size_t KSZ, const size_t TSZ);
	public:
		template<size_t KSZ, size_t TSZ> EEPROMProfileManager(const int baseAddr, const int maxLen, CipherBase &cipher, const uint8_t (&enc_key)[KSZ], uint8_t const (&enc_tweak)[TSZ]) :
		  EEPROMProfileManager(baseAddr,maxLen,cipher, enc_key, enc_tweak, KSZ, TSZ) { }
		template<size_t KSZ, size_t TSZ> EEPROMProfileManager(const int baseAddr, const int maxLen, CipherBase &&, const uint8_t (&&)[KSZ], uint8_t const (&&)[TSZ]) = delete;
		uint16_t GetProfilesCount() final;
		uint16_t GetProfileDataSize() final;
		Profile ReadProfileHeader(uint16_t index) final;
		bool ReadProfileData(uint16_t index, uint8_t * const data) final;
		bool WriteProfile(uint16_t index, const Profile &profile, const uint8_t * const data) final;
};

#endif
