#include "eeprom_settings_manager.h"

template<size_t KN, size_t EN> EEPROMSettingsManager::EEPROMSettingsManager(const uint8_t (&enc_key)[KN], uint8_t const (&enc_tweak)[EN]) :
  key(enc_key),
  tweak(enc_tweak)
{ }

void EEPROMSettingsManager::Commit()
{
 //TODO
}

