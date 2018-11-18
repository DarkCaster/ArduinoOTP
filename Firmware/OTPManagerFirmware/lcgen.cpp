#include "lcgen.h"

LCGen::LCGen(const uint8_t * const seed)
{
    uint64_t init = static_cast<uint32_t>(seed[0]) | static_cast<uint32_t>(seed[1]) << 8 | static_cast<uint32_t>(seed[2]) << 16 | static_cast<uint32_t>(seed[3]) << 24;
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
