#ifndef EEPROM_PROFILE_MANAGER_H
#define EEPROM_PROFILE_MANAGER_H

#include <Arduino.h>
#include "profile_manager.h"
#include "cipher.h"
#include "debug.h"

class EEPROMProfileManager final : public ProfileManager
{
	private:
		const uint8_t * const key;
		const uint8_t * const tweak;
		const size_t keySz;
		const size_t tweakSz;
		const int baseAddr;
		const int maxLen;
		CipherBase &cipher;
	public:
		template<size_t KSZ, size_t TSZ> EEPROMProfileManager(const int baseAddr, const int maxLen, CipherBase &cipher, const uint8_t (&enc_key)[KSZ], uint8_t const (&enc_tweak)[TSZ]) :
		  key(enc_key),
		  tweak(enc_tweak),
		  keySz(cipher.GetKeySize()),
		  tweakSz(cipher.GetTweakSize()),
		  baseAddr(baseAddr),
		  maxLen(maxLen),
		  cipher(cipher)
		{
			if(keySz>KSZ)
				FAIL(100,100);
			if(tweakSz>TSZ)
				FAIL(100,500);
		}
		template<size_t KSZ, size_t TSZ> EEPROMProfileManager(const int baseAddr, const int maxLen, CipherBase &&, const uint8_t (&&)[KSZ], uint8_t const (&&)[TSZ]) = delete;
		uint16_t GetProfilesCount() final;
		uint16_t GetProfileDataSize() final;
		Profile ReadProfileHeader(uint16_t index) final;
		void ReadProfileData(uint16_t index, uint8_t * const data) final;
};

#endif
