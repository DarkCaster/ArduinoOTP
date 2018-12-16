#ifndef EEPROM_WRITER_H
#define EEPROM_WRITER_H

#include <Arduino.h>
#include "cipher.h"

class EEPROMWriter
{
	private:
		int curAddr;
		const int limit;
		Cipher &cipher;
		const uint8_t * const encKey;
		uint8_t * const tweak;
	public:
		EEPROMWriter(const int baseAddr, const int maxLen, Cipher &cipher, const uint8_t * const encKey, uint8_t * const tweak);
		bool WriteNextBlock(const uint8_t * const data);
		bool WriteData(const uint8_t* const data, const uint16_t dLen);
};

#endif
