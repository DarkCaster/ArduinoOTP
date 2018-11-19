#ifndef LCGEN
#define LCGEN

#include <Arduino.h>

class LCGen
{
	private:
		//following constants ensures that LGC overflow will occur in 4294967290 rounds (this gives us 2^32-5-1 uinique values)
		//see http://statmath.wu-wien.ac.at/software/prng/doc/prng.html for constant selection details
		const static uint64_t m = 4294967291UL;
		const static uint64_t a = 279470273UL;
		uint64_t curLCGValue;
	public:
		LCGen(const uint32_t seed);
		uint32_t GenerateValue();
};

#endif
