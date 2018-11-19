#include "lcgen.h"

LCGen::LCGen(const uint32_t seed)
{
	uint64_t init = seed;
	if (init < 1)
		init = 1;
	if (init > (m - 1))
		init = m - 1;
	curLCGValue = init;
}

uint32_t LCGen::GenerateValue()
{
	curLCGValue = (curLCGValue * a) % m;
	return static_cast<uint32_t>(curLCGValue);
}
