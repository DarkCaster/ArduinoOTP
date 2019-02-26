#include "totp_codegen_manager.h"

TOTPCodeGenManager::TOTPCodeGenManager(ClockHelper& clockHelper) :
  clockHelper(clockHelper)
{ }

bool TOTPCodeGenManager::GenerateCode(char * const codeBuff, uint8_t * const dataBuff)
{
	//TODO:
	codeBuff[0]=64;
	codeBuff[1]=0;
	return true;
}

bool TOTPCodeGenManager::VerifySecretData(const uint8_t* const secret, const uint8_t len)
{
	//TODO:
	return true;
}
