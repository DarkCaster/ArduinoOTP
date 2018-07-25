#include "comm_helper.h"
#include "crc8.h"

#ifndef STANDALONE_APP
#define LOG(...) ({})
#endif

Request::Request()
{
  plLen=0;
}

Request::Request(const uint8_t req, const uint8_t* const _payload, const uint8_t _plLen)
{
  reqType= static_cast<ReqType>(req);
  plLen=_plLen;
  for(uint8_t i=0;i<_plLen;++i)
    *(payload+i)=*(_payload+i);
}

Request Request::Invalid()
{
  Request request;
  request.reqType=ReqType::Invalid;
  return request;
}

CommHelper::CommHelper(HardwareSerial * const port) : serial(port) { }

void CommHelper::Init(const long speed)
{
  serial->begin(speed);
  serial->setTimeout(CMD_TIMEOUT);
}

void CommHelper::Deinit()
{
  serial->end();
}

uint8_t CommHelper::DataAvailable()
{
  return serial->available()?1:0;
}

void CommHelper::FlushInput()
{
  while(serial->read()>-1){};
}

#if STANDALONE_APP
#pragma GCC diagnostic ignored "-Wconversion"
#endif

Request CommHelper::ReceiveRequest()
{
  //message buffer
  uint8_t recvBuff[CMD_BUFF_SIZE];
  //read header
  while(!serial->available()){}
  serial->readBytes(recvBuff,1);
  //verify header
  auto remSz = static_cast<uint8_t>(*recvBuff & CMD_SIZE_MASK);
  if(remSz<CMD_MIN_REMSZ||remSz>CMD_MAX_REMSZ)
  {
    LOG("CommHelper::ReceiveRequest: remaining data size is out of bounds");
    return Request::Invalid();
  }
  //check header against supported commands list
  auto req=(uint8_t)(*recvBuff & REQ_ALL_MASK);
  switch(req)
  {
    case REQ_RESYNC:
    case REQ_RESYNC_COMPLETE:
      break;
    default:
      LOG("CommHelper::ReceiveRequest: invalid request received: 0x%02X",req);
      return Request::Invalid();
  }
  //read message-body
  auto startTime=millis();
  auto rem=remSz;
  while(rem>0)
  {
    rem-=static_cast<decltype(rem)>(serial->readBytes(recvBuff+CMD_HDR_SIZE+(remSz-rem),rem));
    if(millis()-startTime>CMD_TIMEOUT)
    {
      LOG("CommHelper::ReceiveRequest: timeout");
      return Request::Invalid();
    }
  }
  //verify CRC
  auto testSz=(uint8_t)(CMD_HDR_SIZE+remSz-1);
  if(*(recvBuff+testSz)!=CRC8(recvBuff,testSz))
  {
    LOG("CommHelper::ReceiveRequest: CRC mismatch!");
    return Request::Invalid();
  }
  //return Request
  return Request(req,recvBuff+CMD_HDR_SIZE,remSz-CMD_CRC_SIZE);
}

#if STANDALONE_APP
#pragma GCC diagnostic pop
#endif

uint8_t CommHelper::SendAnswer(const AnsType answer, const uint8_t* const payload, uint8_t plLen)
{
  if(plLen>CMD_MAX_PLSZ)
    return 0;
  //message buffer
  uint8_t cmdBuff[CMD_BUFF_SIZE];
  //place data to cmdBuff, payload buffer may overlap with cmdBuff
  for(uint8_t i=0; i<plLen; ++i)
    *(cmdBuff+CMD_HDR_SIZE+i)=*(payload+i);
  //write header
#ifdef CMD_HDR_SIZE_IS_1
  *cmdBuff=(uint8_t)(((uint8_t)answer)|(plLen+CMD_CRC_SIZE));
#else
#error unsupporned CMD_HDR_SIZE
#endif
  //write crc
  auto testLen=(uint8_t)(plLen+CMD_HDR_SIZE);
  *(cmdBuff+testLen)=CRC8(cmdBuff,testLen);
  //send data
  auto finalLen=(uint8_t)(testLen+CMD_CRC_SIZE);
  for(uint8_t i=0; i<finalLen; ++i)
    while(serial->write(*(cmdBuff+i))<1);
  return 1;
}
