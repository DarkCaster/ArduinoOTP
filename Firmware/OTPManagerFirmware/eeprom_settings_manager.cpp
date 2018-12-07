#include "eeprom_settings_manager.h"
#include "eeprom_writer.h"
#include "eeprom_reader.h"
#include "crc8.h"
#include "debug.h"

#define CRC_SZ 1

void EEPROMSettingsManager::Commit()
{
	//get pointer (and length) to the settings structure
	const auto sLen=sizeof(Settings);
	uint8_t *sPtr=reinterpret_cast<uint8_t*>(&settings);
	//create EEPROMWriter
	uint8_t tmpTweak[tweakSz];
	memcpy(tmpTweak,tweak,tweakSz);
	EEPROMWriter writer(baseAddr,maxLen,cipher,key,tmpTweak);
	//write settings
	if(!writer.WriteData(sPtr,sLen))
		FAIL(100,2000);
}

void EEPROMSettingsManager::Init()
{
	//get length of the settings structure, create temporary buffer
	const auto sLen=sizeof(Settings);
	uint8_t tmpBuff[sLen];
	//create EEPROMReader
	uint8_t tmpTweak[tweakSz];
	memcpy(tmpTweak,tweak,tweakSz);
	EEPROMReader reader(baseAddr,maxLen,cipher,key,tmpTweak);
	//read data
	auto ec=reader.ReadData(tmpBuff,sLen);
	if(ec<0)
		FAIL(100,2000);
	if(ec==0)
		return;
	  //FAIL(100,5000); //for debug purposes
	//if checksum is OK - save data to the settings struct
	uint8_t * const sPtr=reinterpret_cast<uint8_t*>(&settings);
	memcpy(sPtr,tmpBuff,sLen);
}


