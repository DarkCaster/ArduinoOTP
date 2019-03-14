#include "totp_codegen_manager.h"
#include "hmacsha1.h"

TOTPCodeGenManager::TOTPCodeGenManager(ClockHelper& clockHelper) :
  clockHelper(clockHelper)
{ }

bool TOTPCodeGenManager::GenerateCode(char * const codeBuff, uint8_t * const dataBuff)
{
	//get current time in seconds (including leap years), relative to unix epoch-time and alculate steps from unix epoch
	uint64_t stepsSinceUnixEpoch=clockHelper.GetUnixSeconds()/(*(dataBuff+0)); /*step len in seconds*/
	//get stepsSinceUnixEpoch as bytes in correct endian-format
	uint8_t message[8];
	for (int8_t i = 7; i >= 0; --i)
	{
		*(message+i) = static_cast<uint8_t>(stepsSinceUnixEpoch & 0xFF);
		stepsSinceUnixEpoch >>= 8;
	}
	//get hmac methods for selected hashmode
	hmac_func_ptr hashmac_func=nullptr;
	hmac_getopbuffsz_func_ptr hashmac_getopbuffsz_func=nullptr;
	switch (*(dataBuff+1))
	{
		//hmac-sha1
		case 0:
			hashmac_func=hmacsha1;
			hashmac_getopbuffsz_func=hmacsha1_get_opbuff_sz;
			break;
		default:
			codeBuff[0]=69; //E
			codeBuff[1]=0;
			return false;
	}
	//calculate hmac
	uint8_t opBuff[hashmac_getopbuffsz_func(8)];
	memcpy(opBuff,message,8);
	auto hmacSz=hashmac_func(dataBuff+4/*key*/,*(dataBuff+3)/*keyLen*/,opBuff,8);
	if(hmacSz<20)
	{
		codeBuff[0]=69; //E
		codeBuff[1]=0;
		return false;
	}
	//get OTP password
	uint8_t tOffset=*(opBuff+hmacSz-1) & 0x0F;
	int32_t iCode = (static_cast<int32_t>(*(opBuff+tOffset  ) & 0x7F) << 24) |
	                (static_cast<int32_t>(*(opBuff+tOffset+1) & 0xFF) << 16) |
	                (static_cast<int32_t>(*(opBuff+tOffset+2) & 0xFF) << 8)  |
	                static_cast<int32_t>(*(opBuff+tOffset+3) & 0xFF);
	//truncate result
	uint8_t totpSize=*(dataBuff+2);
	int32_t divisor=1;
	for(uint8_t i=0;i<totpSize;++i)
		divisor*=10;
	iCode%=divisor;
	//write code string
	ltoa(iCode,codeBuff,10);
	//no need to update secret
	return false;
}

bool TOTPCodeGenManager::VerifySecretData(const uint8_t* const secret, const uint8_t len)
{
	//check length
	if(len<5)
		return false;
	//check step (time window)
	if(*(secret+0)>120)
		return false;
	//check hash mode, TODO: add more hash modes (sha256, sha512?)
	if(*(secret+1)!=0)
		return false;
	//check totpSize, max 10 digits allowed by int32_t type that stores result
	if(*(secret+2)>10||*(secret+2)==0)
		return false;
	//check key len
	if(*(secret+3)!=len-4)
		return false;
	return true;
}
