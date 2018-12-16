#include "comm_helper.h"
#include "crc8.h"

#define LOG(...) ({})

static Request RequestCreate(const ReqType req, const uint32_t seq, const uint8_t* const payload, const uint8_t plLen)
{
	Request result; //{}; //full struct init is omited to save some progmem
	result.reqType=req;
	result.seq=seq;
	result.plLen=plLen;
	for(uint8_t i=0;i<plLen;++i)
		*(result.payload+i)=*(payload+i);
	return result;
}

static Request RequestInvalid()
{
	Request result; //{}; //full struct init is omited to save some progmem
	result.reqType=ReqType::Invalid;
	result.seq=0;
	return result;
}

CommHelper::CommHelper(Stream &port) : serial(port) { }

bool CommHelper::DataAvailable()
{
	return serial.available()>0;
}

void CommHelper::FlushInput()
{
	while(serial.read()>-1){};
}

Request CommHelper::ReceiveRequest()
{
	//message buffer
	uint8_t recvBuff[CMD_BUFF_SIZE];
	//read header
	while(!DataAvailable()){}
	serial.readBytes(recvBuff,1);
	//verify header
	auto remSz = static_cast<uint8_t>(*recvBuff & CMD_SIZE_MASK);
	if(remSz<CMD_MIN_REMSZ||remSz>CMD_MAX_REMSZ)
		return RequestInvalid();
	//calculate plSize
	auto plSize=static_cast<uint8_t>(remSz-CMD_CRC_SIZE);
	//check header against supported commands list to verify command validity
	auto req=static_cast<uint8_t>(*recvBuff & REQ_ALL_MASK);
	switch(req)
	{
		//these requests MUST include only 4-byte sequence number only
		case REQ_PING:
		case REQ_RESYNC_COMPLETE:
			if(plSize != CMD_SEQ_SIZE)
				return RequestInvalid();
			break;
		case REQ_COMMAND:
		case REQ_DATA_REQUEST:
			if(plSize != CMD_SEQ_SIZE+1)
				return RequestInvalid();
			break;
		case REQ_RESYNC:
			if(plSize != 0 && plSize != CMD_SEQ_SIZE+CMD_MAX_PLSZ)
				return RequestInvalid();
			break;
		case REQ_COMMAND_DATA:
			if(plSize < CMD_SEQ_SIZE)
				return RequestInvalid();
			break;
		default:
			return RequestInvalid();
	}
	//read message-body
	auto startTime=millis();
	auto rem=remSz;
	while(rem>0)
	{
		rem-=static_cast<decltype(rem)>(serial.readBytes(recvBuff+CMD_HDR_SIZE+(remSz-rem),rem));
		if(millis()-startTime>CMD_TIMEOUT)
			return RequestInvalid();
	}
	//verify CRC
	auto testSz=static_cast<uint8_t>(CMD_HDR_SIZE+remSz-1);
	if(*(recvBuff+testSz)!=CRC8(recvBuff,testSz))
		return RequestInvalid();
	//decode sequence number
	uint32_t seq=0;
	uint8_t* pl=recvBuff+CMD_HDR_SIZE;
	uint8_t dataSize=0;
	if(plSize>=CMD_SEQ_SIZE)
	{
#ifdef CMD_SEQ_SIZE_IS_4
		seq=static_cast<uint32_t>(*pl)|static_cast<uint32_t>(*(pl+1))<<8|static_cast<uint32_t>(*(pl+2))<<16|static_cast<uint32_t>(*(pl+3))<<24;
#else
#error selected CMD_SEQ_SIZE not unsupported
#endif
		dataSize=plSize-CMD_SEQ_SIZE;
	}
	uint8_t* data=(dataSize>0)?pl+CMD_SEQ_SIZE:nullptr;
	//create reqtest-object
	return RequestCreate(static_cast<ReqType>(req),seq,data,dataSize);
}


bool CommHelper::SendAnswer(const AnsType answer, const uint32_t seq, const uint8_t* const payload, uint8_t plLen)
{
	if(plLen>CMD_MAX_PLSZ)
		return false;
	//message buffer
	uint8_t cmdBuff[CMD_BUFF_SIZE];
	uint8_t* plPtr=cmdBuff+CMD_HDR_SIZE;
	//write seq number
	auto seqLen=seq>0?CMD_SEQ_SIZE:0_u8;
	if(seqLen>0)
	{
#ifdef CMD_SEQ_SIZE_IS_4
		*(plPtr)=static_cast<uint8_t>(seq&0xFF);
		*(plPtr+1)=static_cast<uint8_t>((seq>>8)&0xFF);
		*(plPtr+2)=static_cast<uint8_t>((seq>>16)&0xFF);
		*(plPtr+3)=static_cast<uint8_t>((seq>>24)&0xFF);
#else
#error selected CMD_SEQ_SIZE not unsupported
#endif
		plPtr+=seqLen;
	}
	else
		plLen=0;
	//place data to cmdBuff, payload buffer may overlap with cmdBuff
	for(uint8_t i=0; i<plLen; ++i)
		*(plPtr++)=*(payload+i);
	//write header
#ifdef CMD_HDR_SIZE_IS_1
	*cmdBuff=static_cast<uint8_t>((static_cast<uint8_t>(answer))|(seqLen+plLen+CMD_CRC_SIZE));
#else
#error unsupporned CMD_HDR_SIZE
#endif
	//write crc
	auto testLen=static_cast<uint8_t>(plPtr-cmdBuff);
	*plPtr=CRC8(cmdBuff,testLen);
	//send data
	auto finalLen=static_cast<uint8_t>(testLen+CMD_CRC_SIZE);
	for(uint8_t i=0; i<finalLen; ++i)
		while(serial.write(*(cmdBuff+i))<1);
	return true;
}
