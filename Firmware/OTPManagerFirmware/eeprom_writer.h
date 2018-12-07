#ifndef EEPROM_WRITER_H
#define EEPROM_WRITER_H

#include <Arduino.h>
#include "cipher.h"

class EEPROMWriter
{
	private:
		int curAddr;
		const int limit;
		CipherBase &cipher;
		const uint8_t * const encKey;
		uint8_t * const tweak;
	public:
		EEPROMWriter(const int baseAddr, const int maxLen, CipherBase &cipher, const uint8_t * const encKey, uint8_t * const tweak);
		bool WriteNextBlock(const uint8_t * const data);
		bool WriteData(const uint8_t * const data, const size_t size);
};

#endif
