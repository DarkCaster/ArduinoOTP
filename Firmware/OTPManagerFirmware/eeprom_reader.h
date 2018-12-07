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
		int8_t ReadData(uint8_t * const sPtr, const size_t sLen); // -1, eeprom read failed, 0 - crc failed, 1 - ok
};

#endif
