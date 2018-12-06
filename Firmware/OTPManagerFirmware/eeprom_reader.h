#ifndef EEPROM_READER_H
#define EEPROM_READER_H

#include <Arduino.h>
#include "cipher.h"

class EEPROMReader
{
	private:
		int curAddr;
		const int limit;
		CipherBase &cipher;
		const uint8_t * const encKey;
		uint8_t * const tweak;
	public:
		EEPROMReader(const int baseAddr, const int maxLen, CipherBase &cipher, const uint8_t * const encKey, uint8_t * const tweak);
		bool ReadNextBlock(uint8_t * const data);
};

#endif
