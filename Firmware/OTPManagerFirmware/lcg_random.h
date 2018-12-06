#ifndef LCG_RANDOM_H
#define LCG_RANDOM_H

#include <Arduino.h>
#include "lcgen.h"

class LCGRandom
{
	private:
		LCGen lcg;
	public:
		LCGRandom();
		LCGRandom(const uint32_t seed);
		uint32_t Next();
		uint32_t Next(const uint32_t lowerBound, const uint32_t upperBound);
		void NextBytes(uint8_t * const buffer, const size_t cnt);
};

#endif
