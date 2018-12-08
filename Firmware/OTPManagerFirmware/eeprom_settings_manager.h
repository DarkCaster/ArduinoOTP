#ifndef EEPROM_SETTINGS_MANAGER_H
#define EEPROM_SETTINGS_MANAGER_H

#include <Arduino.h>
#include "settings_manager.h"
#include "cipher.h"
#include "debug.h"

class EEPROMSettingsManager final : public SettingsManager
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
		template<size_t KSZ, size_t TSZ> EEPROMSettingsManager(const int baseAddr, const int maxLen, CipherBase &cipher, const uint8_t (&enc_key)[KSZ], uint8_t const (&enc_tweak)[TSZ]) :
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
		template<size_t KSZ, size_t TSZ> EEPROMSettingsManager(const int baseAddr, const int maxLen, CipherBase &&, const uint8_t (&&)[KSZ], uint8_t const (&&)[TSZ]) = delete;
		void Commit() final;
		void Init() final;
};

#endif
