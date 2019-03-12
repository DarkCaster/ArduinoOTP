#include "totp_codegen_manager.h"
#include "hmacsha1.h"

TOTPCodeGenManager::TOTPCodeGenManager(ClockHelper& clockHelper) :
  clockHelper(clockHelper)
{ }

bool TOTPCodeGenManager::GenerateCode(char * const codeBuff, uint8_t * const dataBuff)
{
	//TODO: get current time in seconds (including leap years), relative to unix epoch-time
	uint64_t secondsSinceUnixEpoch=100;
	//calculate steps from unix epoch
	uint64_t stepsSinceUnixEpoch=secondsSinceUnixEpoch/(*(dataBuff+0))/*step len in seconds*/;
	//TODO: get stepsSinceUnixEpoch as bytes in correct endian-format
	uint8_t message[8]={0};
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
	//TODO: truncate hash
	//uint8_t totpSize=*(dataBuff+2);
	//TODO: write code string
	codeBuff[0]=static_cast<char>(opBuff[hmacSz-1]);
	codeBuff[1]=0;
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
	//check hash mode, TODO: add more hash modes
	if(*(secret+1)!=0)
		return false;
	//check totpSize, TODO: max digits also depends on hash mode ?
	if(*(secret+2)<32)
		return false;
	//check key len
	if(*(secret+3)!=len-4)
		return false;
	return true;
}
