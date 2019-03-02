#include "totp_codegen_manager.h"
#include "sha1.h"

TOTPCodeGenManager::TOTPCodeGenManager(ClockHelper& clockHelper) :
  clockHelper(clockHelper)
{ }

bool TOTPCodeGenManager::GenerateCode(char * const codeBuff, uint8_t * const dataBuff)
{
	//TODO:
	uint8_t digest[64];
	sha1(digest,dataBuff,10);
	codeBuff[0]=static_cast<char>(digest[0]);
	codeBuff[1]=0;
	return true;
}

bool TOTPCodeGenManager::VerifySecretData(const uint8_t* const secret, const uint8_t len)
{
	//TODO:
	return true;
}
