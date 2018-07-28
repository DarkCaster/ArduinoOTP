#include "threefish256.h"

uint64_t Threefish256::RotateLeft64(uint64_t v, int8_t b)
{
  return (v << b) | (v >> (64 - b));
}

uint64_t Threefish256::RotateRight64(uint64_t v, int8_t b)
{
  return (v >> b) | (v << (64 - b));
}

void Threefish256::Mix(uint64_t &a, uint64_t &b, int8_t r)
{
  a += b;
  b = RotateLeft64(b, r) ^ a;
}

void Threefish256::Mix(uint64_t &a, uint64_t &b, int8_t r, uint64_t k0, uint64_t k1)
{
  b += k1;
  a += b + k0;
  b = RotateLeft64(b, r) ^ a;
}

void Threefish256::UnMix(uint64_t &a, uint64_t &b, int8_t r)
{
  b = RotateRight64(b ^ a, r);
  a -= b;
}

void Threefish256::UnMix(uint64_t &a, uint64_t &b, int8_t r, uint64_t k0, uint64_t k1)
{
  b = RotateRight64(b ^ a, r);
  a -= b + k0;
  b -= k1;
}

void Threefish256::SetTweak(const uint64_t * const tweak)
{
  ExpandedTweak[0] = tweak[0];
  ExpandedTweak[1] = tweak[1];
  ExpandedTweak[2] = tweak[0] ^ tweak[1];
}

void Threefish256::SetKey(const uint64_t * const key)
{
    uint64_t parity = KeyScheduleConst;
    uint8_t i = 0;
    for (i = 0; i < ExpandedTweakSize - 1; i++)
    {
        *(ExpandedKey+i) = key[i];
        parity ^= key[i];
    }
    *(ExpandedKey+i) = parity;
}
