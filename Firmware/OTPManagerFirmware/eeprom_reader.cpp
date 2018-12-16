#include <EEPROM.h>
#include "eeprom_reader.h"
#include "debug.h"
#include "crc8.h"

#define CRC_SZ 1

uint16_t EEPROMReader::CalculateTotalBlocksCount(const uint16_t sourceDataLen, const uint8_t cipherBsz)
{
	uint16_t totalLen = sourceDataLen+CRC_SZ;
	uint16_t fullBlocks = totalLen/cipherBsz;
	if(totalLen%cipherBsz>0)
		fullBlocks++;
	return fullBlocks;
}

EEPROMReader::EEPROMReader(const int baseAddr, const int maxLen, Cipher& cipher, const uint8_t* const encKey, uint8_t* const tweak) :
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

int8_t EEPROMReader::ReadData(uint8_t * const dPtr, const size_t dLen)
{
	if(!dLen)
		return 1;
	//calculate how much blocks (crc included) we need to read
	const uint8_t bsz=cipher.GetBlockSize();
	auto fullBlocks = CalculateTotalBlocksCount(dLen,bsz);
	//allocate space needed for decrypted block
	uint8_t tmpBuff[bsz];
	size_t dPos=0;
	auto blk=fullBlocks;
	for(blk=0; blk<fullBlocks; ++blk)
	{
		if(!ReadNextBlock(tmpBuff))
			return -1;
		auto remain=dLen-dPos;
		auto pending=bsz>remain?remain:bsz;
		memcpy(dPtr+dPos,tmpBuff,pending);
		dPos+=pending;
	}
	//verify crc
	if(CRC8(dPtr,dLen)!=tmpBuff[bsz-CRC_SZ])
		return 0;
	return 1;
}
