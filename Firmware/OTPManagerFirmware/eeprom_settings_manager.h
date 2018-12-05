#ifndef EEPROM_SETTINGS_MANAGER_H
#define EEPROM_SETTINGS_MANAGER_H

#include <Arduino.h>
#include "settings_manager.h"

class EEPROMSettingsManager final : public SettingsManager
{
	private:
		const uint8_t (&key)[];
		const uint8_t (&tweak)[];
	public:
		template<size_t KN, size_t EN> EEPROMSettingsManager(const uint8_t (&enc_key)[KN], uint8_t const (&enc_tweak)[EN]);
		template<size_t KN, size_t EN> EEPROMSettingsManager(const uint8_t (&&enc_key)[KN], uint8_t const (&&enc_tweak)[EN]) = delete;
		void Commit() final;
};

#endif
