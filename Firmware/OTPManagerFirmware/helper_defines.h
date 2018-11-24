#ifndef HELPER_DEFINES
#define HELPER_DEFINES

#include <Arduino.h>

constexpr uint8_t operator "" _u8 (unsigned long long arg) noexcept
{
	return static_cast<uint8_t>(arg);
}

#endif
