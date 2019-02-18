#include "totp_codegen_manager.h"

TOTPCodeGenManager::TOTPCodeGenManager(ClockHelper& clockHelper) :
  clockHelper(clockHelper)
{ }

uint8_t TOTPCodeGenManager::GenerateCode(uint8_t * const codeBuff, uint8_t * const dataBuff, bool &dataChanged)
{
	//TODO:
	codeBuff[0]=64;
	codeBuff[1]=0;
	dataChanged=false;
	return  0;
}

bool TOTPCodeGenManager::VerifySecretData(const uint8_t* const secret, const uint8_t len)
{
	//TODO:
	return true;
}
