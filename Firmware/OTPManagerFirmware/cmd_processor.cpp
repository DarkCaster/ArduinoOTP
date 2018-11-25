#include "cmd_processor.h"

RspParams RspParams::Empty()
{
	RspParams result;
	result.rspLen=0;
	result.rspType=RSP_EMPTY;
	return result;
}

RspParams RspParams::Invalid()
{
	RspParams result;
	result.rspLen=0;
	result.rspType=RSP_INVALID;
	return result;
}

RspParams RspParams::Error(const CMDRSP_BUFF_TYPE rspLen)
{
	RspParams result;
	result.rspLen=rspLen;
	result.rspType=RSP_ERROR;
	return result;
}

CmdProcessor::CmdProcessor(ClockHelperBase* const _clockHelper) : clockHelper(_clockHelper) { }

RspParams CmdProcessor::ProcessCommand(const uint8_t cmdType, const uint8_t* const cmdData, const CMDRSP_BUFF_TYPE cmdLen, uint8_t* const rspData)
{
	//check cmdType
	RspParams result;
	switch (cmdType)
	{
		case CMD_SETTIME:
			result=SetTime(cmdData,cmdLen);
			break;
		default:
			return RspParams::Invalid();
	}
	return result;
}

RspParams CmdProcessor::SetTime(const uint8_t* const cmdData, const CMDRSP_BUFF_TYPE cmdLen)
{
	if(cmdLen!=10)
		return RspParams::Invalid();
	CMDRSP_BUFF_TYPE pos=0;
	uint8_t sec=cmdData[pos++];
	uint8_t min=cmdData[pos++];
	uint8_t hour=cmdData[pos++];
	uint8_t day=cmdData[pos]&0x1F;
	uint8_t dow=cmdData[pos++]>>5;
	uint8_t month=cmdData[pos++];
	uint16_t year=cmdData[pos++];
	year|=static_cast<uint16_t>(cmdData[pos++])<<5;
	uint32_t utcOffset=cmdData[pos++];
	utcOffset|=static_cast<uint32_t>(cmdData[pos++])<<8;
	utcOffset|=static_cast<uint32_t>(cmdData[pos++])<<16;
	//TODO: use clockHelper to set time
	if(!clockHelper->SetTime(sec,min,hour,day,dow,month,year,utcOffset))
		return RspParams::Error(0);
	return RspParams::Empty();
}
