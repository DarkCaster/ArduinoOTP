#include "eeprom_settings_manager.h"
#include "eeprom_writer.h"
#include "crc8.h"
#include "debug.h"

#define CRC_SZ 1

void EEPROMSettingsManager::Commit()
{
	//get pointer (and length) to the settings structure
	const auto sLen=sizeof(Settings);
	if(!sLen)
		return;
	uint8_t *sPtr=reinterpret_cast<uint8_t*>(&settings);
	//calculate how much blocks we need to write
	auto bsz=cipher.GetBlockSize();
	auto fullBlocks = (sLen+CRC_SZ) / bsz;
	auto remainingBytes = (sLen+CRC_SZ) % bsz;
	//fillup last block and write checksum
	uint8_t lastBlock[bsz];
	if(!remainingBytes)
	{
		fullBlocks--;
		memcpy(lastBlock, sPtr+fullBlocks*bsz, bsz-CRC_SZ);
	}
	else
	{
		memset(lastBlock, 0, bsz);
		memcpy(lastBlock, sPtr+fullBlocks*bsz, remainingBytes);
	}
#if CRC_SZ == 1
	//write crc
	*(lastBlock+bsz-1)=CRC8(sPtr,sLen);
#else
#error TODO
#endif
	//create EEPROMWriter
	uint8_t tmpTweak[tweakSz];
	memcpy(tmpTweak,tweak,tweakSz);
	EEPROMWriter writer(baseAddr,maxLen,cipher,key,tmpTweak);
	//write settings block-by-block
	auto blk=fullBlocks;
#if DEBUG
	for(blk=0; blk<fullBlocks; ++blk)
	{
		if(!writer.WriteNextBlock(sPtr+blk*bsz))
			FAIL(100,2000);
	}
	if(writer.WriteNextBlock(lastBlock))
		FAIL(100,2000);
#else
	for(blk=0; blk<fullBlocks; ++blk)
		writer.WriteNextBlock(sPtr+blk*bsz);
	writer.WriteNextBlock(lastBlock);
#endif
}

void EEPROMSettingsManager::Init()
{
	//TODO:
}


