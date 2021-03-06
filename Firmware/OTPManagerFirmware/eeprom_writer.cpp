#include <EEPROM.h>
#include "eeprom_writer.h"
#include "debug.h"
#include "crc8.h"

#define CRC_SZ 1

EEPROMWriter::EEPROMWriter(const int baseAddr, const int maxLen, Cipher& cipher, const uint8_t* const encKey, uint8_t* const tweak) :
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
	//CBC - use current encrypted data as IV in the next encrypt operation
	memcpy(tweak,encData,cipher.GetTweakSize());
	//write data to eeprom
	for(uint8_t bp=0; bp<bsz; ++bp)
		EEPROM.update(curAddr++,*(encData+bp));
	return true;
}

bool EEPROMWriter::WriteData(const uint8_t* const data, const uint16_t dLen)
{
	if(dLen==0)
		  return true;
	//calculate how much blocks we need to write
	const uint8_t bsz=cipher.GetBlockSize();
	uint16_t fullBlocks = (dLen+CRC_SZ) / bsz;
	const uint16_t remainingBytes = (dLen+CRC_SZ) % bsz;
	//fillup last block and write checksum
	uint8_t lastBlock[bsz];
	if(remainingBytes == 0)
	{
		fullBlocks--;
		memcpy(lastBlock, data+fullBlocks*bsz, bsz-CRC_SZ);
	}
	else
	{
		for(uint8_t b=0; b<bsz; ++b)
			*(lastBlock+b)=0;
		memcpy(lastBlock, data+fullBlocks*bsz, remainingBytes);
	}
#if CRC_SZ == 1
	//write crc
	*(lastBlock+bsz-1)=CRC8(data,dLen);
#else
#error TODO
#endif
	//write settings block-by-block
	for(uint16_t blk=0; blk<fullBlocks; ++blk)
		if(!WriteNextBlock(data+blk*bsz))
			return false;
	return WriteNextBlock(lastBlock);
}
