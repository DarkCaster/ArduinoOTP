#include "eeprom_writer.h"
#include "debug.h"
#include "EEPROM.h"

EEPROMWriter::EEPROMWriter(const int baseAddr, const int maxLen, CipherBase& cipher, const uint8_t* const encKey, uint8_t* const tweak) :
  curAddr(baseAddr),
  limit(baseAddr+maxLen),
  cipher(cipher),
  encKey(encKey),
  tweak(tweak)
{
	if(curAddr>limit)
		FAIL(100,500);
	if(limit>static_cast<int>(EEPROM.length()))
		FAIL(100,1000);
}

bool EEPROMWriter::WriteNextBlock(const uint8_t* const data)
{
	const uint8_t bsz=cipher.GetBlockSize();
	//create buffer for output data
	uint8_t encData[bsz];
	//encrypt block
	cipher.EncryptBlock(data,encData,encKey,tweak);
	//check, that we will not write outsize bounds
	if(limit-curAddr<bsz)
		return false;
	//modify tweak array
	cipher.MixTweak(encData,tweak);
	//write data to eeprom
	for(uint8_t bp=0; bp<bsz; ++bp)
		EEPROM.update(curAddr++,*(encData+bp));
	return true;
}
