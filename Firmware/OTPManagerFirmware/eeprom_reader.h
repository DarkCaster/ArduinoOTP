#ifndef EEPROM_READER_H
#define EEPROM_READER_H

#include <Arduino.h>
#include "cipher.h"

class EEPROMReader
{
	private:
		int curAddr;
		const int limit;
		Cipher &cipher;
		const uint8_t * const encKey;
		uint8_t * const tweak;
	public:
		EEPROMReader(const int baseAddr, const int maxLen, Cipher &cipher, const uint8_t * const encKey, uint8_t * const tweak);
		bool ReadNextBlock(uint8_t * const data);
		int8_t ReadData(uint8_t *dPtr, uint16_t dLen); // -1, eeprom read failed, 0 - crc failed, 1 - ok
		static uint16_t CalculateTotalBlocksCount(const uint16_t sourceDataLen, const uint8_t cipherBsz); //helper method, used to share more common code
};

#endif
