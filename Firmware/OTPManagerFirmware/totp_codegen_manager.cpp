#include "totp_codegen_manager.h"

TOTPCodeGenManager::TOTPCodeGenManager(ClockHelper& clockHelper) :
  clockHelper(clockHelper)
{ }

uint8_t TOTPCodeGenManager::GenerateCode(uint8_t* codeBuff, const uint8_t* const dataBuff)
{
	//TODO:
	return  0;
}

bool TOTPCodeGenManager::VerifySecretData(const uint8_t* const secret, const uint8_t len)
{
	//TODO:
	return true;
}
