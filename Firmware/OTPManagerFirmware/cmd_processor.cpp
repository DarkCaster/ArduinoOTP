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

RspParams CmdProcessor::ProcessCommand(const uint8_t cmdType, const uint8_t* const cmdData, const uint8_t cmdLen, uint8_t* const rspData)
{
	//check cmdType
	//process command
	//return result
	return RspParams::Invalid();
}

