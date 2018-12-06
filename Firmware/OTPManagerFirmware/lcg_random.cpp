#include "lcg_random.h"

LCGRandom::LCGRandom() : lcg(0) { }

LCGRandom::LCGRandom(const uint32_t seed) : lcg(seed) { }

uint32_t LCGRandom::Next() { return lcg.GenerateValue(); }

uint32_t LCGRandom::Next(const uint32_t lowerBound, const uint32_t upperBound)
{
	constexpr static const double UINT32_DSC = 1.0/(static_cast<double>(UINT32_MAX) + 1.0);
	if(lowerBound >= upperBound)
		return lowerBound;
	return  lowerBound + static_cast<uint32_t>(UINT32_DSC * static_cast<double>(lcg.GenerateValue()) * static_cast<double>(upperBound-lowerBound));
}

void LCGRandom::NextBytes(uint8_t* const buffer, const size_t cnt)
{
	for(size_t i=0; i<cnt; ++i)
		*(buffer+i)=static_cast<uint8_t>(Next(0,256));
}

