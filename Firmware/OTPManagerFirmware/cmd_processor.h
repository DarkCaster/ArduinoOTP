#ifndef CMDPROCESSOR_H
#define CMDPROCESSOR_H

#include <Arduino.h>
#include "configuration.h"
#include "helper_defines.h"
#include "clock_helper.h"

#define RSP_EMPTY 0_u8
#define RSP_INVALID 1_u8

struct RspParams
{
		CMDRSP_BUFF_TYPE rspLen;
		uint8_t rspType;
		static RspParams Empty();
		static RspParams Invalid();
};

class CmdProcessor
{
	private:
		ClockHelperBase * const clockHelper;
		RspParams SetTime(const uint8_t* const cmdData, const CMDRSP_BUFF_TYPE cmdLen);
	public:
		CmdProcessor(ClockHelperBase * const clockHelper);
		RspParams ProcessCommand(const uint8_t cmdType, const uint8_t * const cmdData, const CMDRSP_BUFF_TYPE cmdLen, uint8_t * const rspData);
};

#endif // CMDPROCESSOR_H
