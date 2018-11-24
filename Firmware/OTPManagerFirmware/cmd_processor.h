#ifndef CMDPROCESSOR_H
#define CMDPROCESSOR_H

#include <Arduino.h>
#include "configuration.h"
#include "helper_defines.h"

#define RSP_EMPTY 0_u8

struct RspParams
{
		CMDRSP_BUFF_TYPE rspLen;
		uint8_t rspType;
		static RspParams Empty();
};

class CmdProcessor
{
	public:
		static RspParams ProcessCommand(const uint8_t cmdType, const uint8_t * const cmdData, const CMDRSP_BUFF_TYPE cmdLen, uint8_t * const rspData);
};

#endif // CMDPROCESSOR_H
