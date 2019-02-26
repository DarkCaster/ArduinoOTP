#ifndef CODEGEN_MANAGER_H
#define CODEGEN_MANAGER_H

#include <Arduino.h>
#include "profile.h"

class CodeGenManager
{
	public:
		virtual bool GenerateCode(char * const codeBuff, uint8_t * const dataBuff) = 0;
		virtual bool VerifySecretData(const uint8_t * const secret, const CMDRSP_BUFF_TYPE len) = 0;
};

struct CodeGenConfig
{
	const ProfileType type;
	CodeGenManager& generator;
	CodeGenConfig( const CodeGenConfig& other ) = default;
};

#endif
