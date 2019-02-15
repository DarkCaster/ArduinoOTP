#ifndef TOTP_CODEGEN_MANAGER_H
#define TOTP_CODEGEN_MANAGER_H

#include <Arduino.h>
#include "codegen_manager.h"
#include "clock_helper.h"

class TOTPCodeGenManager final : public CodeGenManager
{
	private:
		ClockHelper &clockHelper;
	public:
		TOTPCodeGenManager(ClockHelper &clockHelper);
		TOTPCodeGenManager(ClockHelper &&) = delete;
		uint8_t GenerateCode(uint8_t * codeBuff, const uint8_t * const dataBuff) final;
};

#endif
