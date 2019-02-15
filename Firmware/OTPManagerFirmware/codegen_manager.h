#ifndef CODEGEN_MANAGER_H
#define CODEGEN_MANAGER_H

#include <Arduino.h>

class CodeGenManager
{
	public:
		virtual uint8_t GenerateCode(uint8_t * codeBuff, const uint8_t * const dataBuff) = 0;
};

struct CodeGenConfig
{
	const uint8_t type;
	CodeGenManager& generator;
	CodeGenConfig(uint8_t type, CodeGenManager& generator);
	//CodeGenConfig( const CodeGenConfig& other ) = default;
};

#endif
