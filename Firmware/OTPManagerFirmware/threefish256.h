#ifndef THREEFISH256_H
#define THREEFISH256_H

#include <Arduino.h>

// based on https://github.com/nitrocaster/SkeinFish implementation, MIT-license

class Threefish256
{
  private:
    static const uint64_t KeyScheduleConst = 0x1BD11BDAA9FC1A22;
    static const uint8_t ExpandedTweakSize = 3;
    uint64_t ExpandedTweak[ExpandedTweakSize];
    uint64_t ExpandedKey[ExpandedTweakSize];

    static uint64_t RotateLeft64(uint64_t v, int8_t b);
    static uint64_t RotateRight64(uint64_t v, int8_t b);
    static void Mix(uint64_t &a, uint64_t &b, int8_t r);
    static void Mix(uint64_t &a, uint64_t &b, int8_t r, uint64_t k0, uint64_t k1);
    static void UnMix(uint64_t &a, uint64_t &b, int8_t r);
    static void UnMix(uint64_t &a, uint64_t &b, int8_t r, uint64_t k0, uint64_t k1);

    void SetTweak(const uint64_t * const tweak);
    void SetKey(const uint64_t * const key);


};

#endif
