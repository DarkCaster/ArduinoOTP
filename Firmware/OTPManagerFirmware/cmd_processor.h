#ifndef CMDPROCESSOR_H
#define CMDPROCESSOR_H

#include <Arduino.h>
#include "configuration.h"
#include "helper_defines.h"
#include "clock_helper.h"
#include "profile_manager.h"

#define RSP_EMPTY 0_u8
#define RSP_INVALID 1_u8
#define RSP_ERROR 2_u8
#define RSP_PRCOUNT 3_u8
#define RSP_PROFILE 4_u8

#define CMD_SETTIME 1_u8
#define CMD_GETPRCOUNT 2_u8
#define CMD_GETPROFILE 3_u8

struct RspParams
{
		CMDRSP_BUFF_TYPE rspLen;
		uint8_t rspType;
		static RspParams Empty();
		static RspParams Invalid();
		static RspParams Error(const CMDRSP_BUFF_TYPE rspLen);
};

class CmdProcessor
{
	private:
		ClockHelperBase &clockHelper;
		ProfileManager &profileManager;
		RspParams SetTime(const uint8_t* const cmdData, const CMDRSP_BUFF_TYPE cmdLen);
		RspParams GetProfilesCount(const CMDRSP_BUFF_TYPE cmdLen, uint8_t * const rspData);
		RspParams GetProfile(const uint8_t * const cmdData, const CMDRSP_BUFF_TYPE cmdLen, uint8_t * const rspData);
	public:
		CmdProcessor(ClockHelperBase &clockHelper, ProfileManager &profileManager);
		CmdProcessor(ClockHelperBase &&) = delete;
		RspParams ProcessCommand(const uint8_t cmdType, const uint8_t * const cmdData, const CMDRSP_BUFF_TYPE cmdLen, uint8_t * const rspData);
};

#endif // CMDPROCESSOR_H
