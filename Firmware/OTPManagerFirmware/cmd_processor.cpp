#include "cmd_processor.h"

RspParams RspParams::Empty()
{
	RspParams result{};
	result.rspLen=0;
	result.rspType=RSP_EMPTY;
	return result;
}

RspParams RspParams::Invalid()
{
	RspParams result{};
	result.rspLen=0;
	result.rspType=RSP_INVALID;
	return result;
}

RspParams RspParams::Error(const CMDRSP_BUFF_TYPE rspLen)
{
	RspParams result{};
	result.rspLen=rspLen;
	result.rspType=RSP_ERROR;
	return result;
}

CmdProcessor::CmdProcessor(ClockHelper &clockHelper, ProfileManager &profileManager, const CodeGenConfig * const codeGenConfigs, const size_t CSZ) :
  codeGenConfigs(codeGenConfigs),
  codeGenConfigsSz(CSZ),
  clockHelper(clockHelper),
  profileManager(profileManager)
{ }

RspParams CmdProcessor::ProcessCommand(const uint8_t cmdType, const uint8_t* const cmdData, const CMDRSP_BUFF_TYPE cmdLen, uint8_t* const rspData)
{
	switch (cmdType)
	{
		case CMD_SETTIME:
			return SetTime(cmdData,cmdLen);
		case CMD_GETPRCOUNT:
			return GetProfilesCount(cmdLen,rspData);
		case CMD_GETPROFILE:
			return GetProfile(cmdData,cmdLen,rspData);
		default:
			return RspParams::Invalid();
	}
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

RspParams CmdProcessor::GetProfile(const uint8_t* const cmdData, const CMDRSP_BUFF_TYPE cmdLen, uint8_t * const rspData)
{
	if(cmdLen!=2)
		return RspParams::Invalid();
	auto index=static_cast<uint16_t>( static_cast<uint16_t>(*cmdData) | static_cast<uint16_t>(*(cmdData+1))<<8 );
	auto profile=profileManager.ReadProfileHeader(index);
	uint8_t nameLen=0;
	//poor-man's strnlen, to save some tiny bit of progmem
	//this is not an equalent to strnlen, will set nameLen to 0 if null character not found
	for(uint8_t pos=0; pos<PROFILE_NAME_LEN; ++pos)
		if(*(profile.name+pos)=='\0')
		{
			nameLen=pos;
			break;
		}
	*(rspData)=static_cast<uint8_t>(profile.type);
	memcpy(rspData+1,profile.name,nameLen);
	RspParams result{};
	result.rspLen=nameLen+1;
	result.rspType=RSP_PROFILE;
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
