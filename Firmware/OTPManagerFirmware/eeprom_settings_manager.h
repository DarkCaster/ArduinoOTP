#ifndef EEPROM_SETTINGS_MANAGER_H
#define EEPROM_SETTINGS_MANAGER_H

#include <Arduino.h>
#include "settings_manager.h"
#include "cipher.h"

class EEPROMSettingsManager final : public SettingsManager
{
	private:
		const uint8_t * const key;
		const uint8_t * const tweak;
		const size_t keySz;
		const size_t tweakSz;
		const CipherBase &cipher;
	public:
		template<size_t KSZ, size_t TSZ> EEPROMSettingsManager(const uint8_t (&enc_key)[KSZ], uint8_t const (&enc_tweak)[TSZ], const CipherBase &cipher) :
		  key(enc_key),
		  tweak(enc_tweak),
		  keySz(KSZ),
		  tweakSz(TSZ),
		  cipher(cipher) { }
		template<size_t KSZ, size_t TSZ> EEPROMSettingsManager(const uint8_t (&&enc_key)[KSZ], uint8_t const (&&enc_tweak)[TSZ], const CipherBase &&cipher) = delete;
		void Commit() final;
};

#endif
