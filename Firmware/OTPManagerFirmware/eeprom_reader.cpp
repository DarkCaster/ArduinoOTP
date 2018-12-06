#include "eeprom_reader.h"
#include "EEPROM.h"
#include "debug.h"

EEPROMReader::EEPROMReader(const int baseAddr, const int maxLen, CipherBase& cipher, const uint8_t* const encKey, uint8_t* const tweak) :
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
	if(cipher.GetTweakSize()>cipher.GetBlockSize()) // for cbc to work tweak(iv) size must be lower than block size
		FAIL(100,5000);
}

bool EEPROMReader::ReadNextBlock(uint8_t* const data)
{
	const uint8_t bsz=cipher.GetBlockSize();
	//check, that we will not read outsize bounds
	if(limit-curAddr<bsz)
		return false;
	//create buffer for incoming encrypted data
	uint8_t encData[bsz];
	//read data from eeprom
	for(uint8_t bp=0; bp<bsz; ++bp)
		*(encData+bp)=EEPROM.read(curAddr++);
	//decrypt, and write to target buffer
	cipher.DecryptBlock(encData,data,encKey,tweak);
	//CBC - use current encrypted data as IV in the next encrypt operation
	memcpy(tweak,encData,cipher.GetTweakSize());
	return true;
}
