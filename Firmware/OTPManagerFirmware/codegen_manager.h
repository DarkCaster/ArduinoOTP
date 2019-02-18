#ifndef CODEGEN_MANAGER_H
#define CODEGEN_MANAGER_H

#include <Arduino.h>
#include "profile.h"

class CodeGenManager
{
	public:
		virtual uint8_t GenerateCode(uint8_t * const codeBuff, uint8_t * const dataBuff, bool &dataChanged) = 0;
		virtual bool VerifySecretData(const uint8_t * const secret, const CMDRSP_BUFF_TYPE len) = 0;
};

struct CodeGenConfig
{
	const ProfileType type;
	CodeGenManager& generator;
	CodeGenConfig( const CodeGenConfig& other ) = default;
};

#endif
