#ifndef COMM_HELPER
#define COMM_HELPER

#include <Arduino.h>

constexpr uint8_t operator "" _u8 (unsigned long long arg) noexcept
{
  return static_cast<uint8_t>(arg);
}

// command buffer sizes
#define CMD_HDR_SIZE 1_u8
#define CMD_HDR_SIZE_IS_1
#define CMD_CRC_SIZE 1_u8
#define CMD_BUFF_SIZE 16 //1 byte - header, up to 14 bytes - payload, 1 byte - crc8 checksum
#define CMD_TIMEOUT 500 //timeout for reading command payload

#define CMD_SIZE_MASK 0x1F_u8
#define CMD_MAX_REMSZ 15_u8
#define CMD_MIN_REMSZ 1_u8
//#define CMD_MAX_PLSZ  14_u8
#define CMD_MAX_PLSZ  10_u8

// requests (masks)
#define REQ_ALL_MASK 0xE0_u8
#define REQ_INVALID 0x00_u8
#define REQ_PING 0x20_u8
#define REQ_COMMAND 0x40_u8
#define REQ_COMMAND_DATA 0x60_u8
#define REQ_DATA_REQUEST 0x80_u8
#define REQ_RESYNC_COMPLETE 0xC0_u8
#define REQ_RESYNC 0xE0_u8

// answers (masks)
#define ANS_ALL_MASK 0xE0_u8
#define ANS_INVALID 0x00_u8
#define ANS_PONG 0x20_u8
#define ANS_DATA_MARKER 0x40_u8
#define ANS_DATA 0x60_u8
#define ANS_OK 0xC0_u8
#define ANS_RESYNC 0xE0_u8

enum class ReqType : uint8_t
{
	Invalid = REQ_INVALID,
	Ping = REQ_PING,
	Command = REQ_COMMAND,
	CommandData = REQ_COMMAND_DATA,
	DataRequest = REQ_DATA_REQUEST,
	ResyncComplete = REQ_RESYNC_COMPLETE,
	Resync = REQ_RESYNC
};

enum class AnsType : uint8_t
{
	Invalid = ANS_INVALID,
	Pong = ANS_PONG,
	DataMarker = ANS_DATA_MARKER,
	Data = ANS_DATA,
	Ok = ANS_OK,
	Resync = ANS_RESYNC
};

struct Request
{
	private:
		Request();
	public:
		static Request Invalid();
		static Request Invalid(const uint8_t plLen);
		Request(const uint8_t req, const uint32_t seq, const uint8_t* const payload, const uint8_t plLen);
		ReqType reqType;
		uint32_t seq;
		uint8_t plLen;
		uint8_t payload[CMD_MAX_PLSZ];
};

class CommHelper
{
  private:
    HardwareSerial * const serial;
  public:
    CommHelper(HardwareSerial * const port);
    void Init(const long speed);
    void Deinit();
    uint8_t DataAvailable();
    void FlushInput();
    Request ReceiveRequest();
    uint8_t SendAnswer(const AnsType answer, const uint8_t* const payload, uint8_t plLen);
};

#endif
