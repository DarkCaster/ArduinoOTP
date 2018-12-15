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

CmdProcessor::CmdProcessor(ClockHelperBase &clockHelper, ProfileManager &profileManager) :
  clockHelper(clockHelper),
  profileManager(profileManager)
{ }

RspParams CmdProcessor::ProcessCommand(const uint8_t cmdType, const uint8_t* const cmdData, const CMDRSP_BUFF_TYPE cmdLen, uint8_t* const rspData)
{
	//check cmdType
	RspParams result;
	switch (cmdType)
	{
		case CMD_SETTIME:
			result=SetTime(cmdData,cmdLen);
			break;
		case CMD_GETPRCOUNT:
			result=GetProfilesCount(cmdLen,rspData);
			break;
		default:
			return RspParams::Invalid();
	}
	return result;
}

RspParams CmdProcessor::GetProfilesCount(const CMDRSP_BUFF_TYPE cmdLen, uint8_t * const rspData)
{
	if(cmdLen!=0)
		return RspParams::Invalid();
	uint16_t prCount=profileManager.GetProfilesCount();
	*(rspData)=static_cast<uint8_t>(prCount & 0xFF);
	*(rspData+1)=static_cast<uint8_t>((prCount>>8) & 0xFF);
	auto result=RspParams();
	result.rspLen=2;
	result.rspType=RSP_PRCOUNT;
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
	year|=(static_cast<uint16_t>(cmdData[pos++])<<8);

	int32_t utcOffset=cmdData[pos++];
	utcOffset|=static_cast<int32_t>(cmdData[pos++])<<8;
	uint8_t sign=cmdData[pos]>>7;
	utcOffset|=static_cast<int32_t>(cmdData[pos++]&0x7F)<<16;
	if(sign>0)
		utcOffset=-utcOffset;
	if(!clockHelper.SetTime(sec,min,hour,day,dow,month,year,utcOffset))
		return RspParams::Error(0);
	return RspParams::Empty();
}
