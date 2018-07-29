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
    for (i = 0; i < ExpandedKeySize - 1; i++)
    {
        *(ExpandedKey+i) = key[i];
        parity ^= key[i];
    }
    *(ExpandedKey+i) = parity;
}

void Threefish256::Encrypt(const uint64_t * const input, uint64_t * const output)
{
  // Cache the block, key, and tweak
  uint64_t b0 = input[0], b1 = input[1],
      b2 = input[2], b3 = input[3];
  uint64_t k0 = ExpandedKey[0], k1 = ExpandedKey[1],
      k2 = ExpandedKey[2], k3 = ExpandedKey[3],
      k4 = ExpandedKey[4];
  uint64_t t0 = ExpandedTweak[0], t1 = ExpandedTweak[1],
      t2 = ExpandedTweak[2];

  Mix( b0,  b1, 14, k0, k1 + t0);
  Mix(b2, b3, 16, k2 + t1, k3);
  Mix(b0, b3, 52);
  Mix(b2, b1, 57);
  Mix(b0, b1, 23);
  Mix(b2, b3, 40);
  Mix(b0, b3, 5);
  Mix(b2, b1, 37);
  Mix(b0, b1, 25, k1, k2 + t1);
  Mix(b2, b3, 33, k3 + t2, k4 + 1);
  Mix(b0, b3, 46);
  Mix(b2, b1, 12);
  Mix(b0, b1, 58);
  Mix(b2, b3, 22);
  Mix(b0, b3, 32);
  Mix(b2, b1, 32);
  Mix(b0, b1, 14, k2, k3 + t2);
  Mix(b2, b3, 16, k4 + t0, k0 + 2);
  Mix(b0, b3, 52);
  Mix(b2, b1, 57);
  Mix(b0, b1, 23);
  Mix(b2, b3, 40);
  Mix(b0, b3, 5);
  Mix(b2, b1, 37);
  Mix(b0, b1, 25, k3, k4 + t0);
  Mix(b2, b3, 33, k0 + t1, k1 + 3);
  Mix(b0, b3, 46);
  Mix(b2, b1, 12);
  Mix(b0, b1, 58);
  Mix(b2, b3, 22);
  Mix(b0, b3, 32);
  Mix(b2, b1, 32);
  Mix(b0, b1, 14, k4, k0 + t1);
  Mix(b2, b3, 16, k1 + t2, k2 + 4);
  Mix(b0, b3, 52);
  Mix(b2, b1, 57);
  Mix(b0, b1, 23);
  Mix(b2, b3, 40);
  Mix(b0, b3, 5);
  Mix(b2, b1, 37);
  Mix(b0, b1, 25, k0, k1 + t2);
  Mix(b2, b3, 33, k2 + t0, k3 + 5);
  Mix(b0, b3, 46);
  Mix(b2, b1, 12);
  Mix(b0, b1, 58);
  Mix(b2, b3, 22);
  Mix(b0, b3, 32);
  Mix(b2, b1, 32);
  Mix(b0, b1, 14, k1, k2 + t0);
  Mix(b2, b3, 16, k3 + t1, k4 + 6);
  Mix(b0, b3, 52);
  Mix(b2, b1, 57);
  Mix(b0, b1, 23);
  Mix(b2, b3, 40);
  Mix(b0, b3, 5);
  Mix(b2, b1, 37);
  Mix(b0, b1, 25, k2, k3 + t1);
  Mix(b2, b3, 33, k4 + t2, k0 + 7);
  Mix(b0, b3, 46);
  Mix(b2, b1, 12);
  Mix(b0, b1, 58);
  Mix(b2, b3, 22);
  Mix(b0, b3, 32);
  Mix(b2, b1, 32);
  Mix(b0, b1, 14, k3, k4 + t2);
  Mix(b2, b3, 16, k0 + t0, k1 + 8);
  Mix(b0, b3, 52);
  Mix(b2, b1, 57);
  Mix(b0, b1, 23);
  Mix(b2, b3, 40);
  Mix(b0, b3, 5);
  Mix(b2, b1, 37);
  Mix(b0, b1, 25, k4, k0 + t0);
  Mix(b2, b3, 33, k1 + t1, k2 + 9);
  Mix(b0, b3, 46);
  Mix(b2, b1, 12);
  Mix(b0, b1, 58);
  Mix(b2, b3, 22);
  Mix(b0, b3, 32);
  Mix(b2, b1, 32);
  Mix(b0, b1, 14, k0, k1 + t1);
  Mix(b2, b3, 16, k2 + t2, k3 + 10);
  Mix(b0, b3, 52);
  Mix(b2, b1, 57);
  Mix(b0, b1, 23);
  Mix(b2, b3, 40);
  Mix(b0, b3, 5);
  Mix(b2, b1, 37);
  Mix(b0, b1, 25, k1, k2 + t2);
  Mix(b2, b3, 33, k3 + t0, k4 + 11);
  Mix(b0, b3, 46);
  Mix(b2, b1, 12);
  Mix(b0, b1, 58);
  Mix(b2, b3, 22);
  Mix(b0, b3, 32);
  Mix(b2, b1, 32);
  Mix(b0, b1, 14, k2, k3 + t0);
  Mix(b2, b3, 16, k4 + t1, k0 + 12);
  Mix(b0, b3, 52);
  Mix(b2, b1, 57);
  Mix(b0, b1, 23);
  Mix(b2, b3, 40);
  Mix(b0, b3, 5);
  Mix(b2, b1, 37);
  Mix(b0, b1, 25, k3, k4 + t1);
  Mix(b2, b3, 33, k0 + t2, k1 + 13);
  Mix(b0, b3, 46);
  Mix(b2, b1, 12);
  Mix(b0, b1, 58);
  Mix(b2, b3, 22);
  Mix(b0, b3, 32);
  Mix(b2, b1, 32);
  Mix(b0, b1, 14, k4, k0 + t2);
  Mix(b2, b3, 16, k1 + t0, k2 + 14);
  Mix(b0, b3, 52);
  Mix(b2, b1, 57);
  Mix(b0, b1, 23);
  Mix(b2, b3, 40);
  Mix(b0, b3, 5);
  Mix(b2, b1, 37);
  Mix(b0, b1, 25, k0, k1 + t0);
  Mix(b2, b3, 33, k2 + t1, k3 + 15);
  Mix(b0, b3, 46);
  Mix(b2, b1, 12);
  Mix(b0, b1, 58);
  Mix(b2, b3, 22);
  Mix(b0, b3, 32);
  Mix(b2, b1, 32);
  Mix(b0, b1, 14, k1, k2 + t1);
  Mix(b2, b3, 16, k3 + t2, k4 + 16);
  Mix(b0, b3, 52);
  Mix(b2, b1, 57);
  Mix(b0, b1, 23);
  Mix(b2, b3, 40);
  Mix(b0, b3, 5);
  Mix(b2, b1, 37);
  Mix(b0, b1, 25, k2, k3 + t2);
  Mix(b2, b3, 33, k4 + t0, k0 + 17);
  Mix(b0, b3, 46);
  Mix(b2, b1, 12);
  Mix(b0, b1, 58);
  Mix(b2, b3, 22);
  Mix(b0, b3, 32);
  Mix(b2, b1, 32);

  output[0] = b0 + k3;
  output[1] = b1 + k4 + t0;
  output[2] = b2 + k0 + t1;
  output[3] = b3 + k1 + 18;
}

void Threefish256::Decrypt(const uint64_t * const input, uint64_t * const output)
{
  // Cache the block, key, and tweak
  uint64_t b0 = input[0], b1 = input[1],
      b2 = input[2], b3 = input[3];
  uint64_t k0 = ExpandedKey[0], k1 = ExpandedKey[1],
      k2 = ExpandedKey[2], k3 = ExpandedKey[3],
      k4 = ExpandedKey[4];
  uint64_t t0 = ExpandedTweak[0], t1 = ExpandedTweak[1],
      t2 = ExpandedTweak[2];

  b0 -= k3;
  b1 -= k4 + t0;
  b2 -= k0 + t1;
  b3 -= k1 + 18;
  UnMix(b0, b3, 32);
  UnMix(b2, b1, 32);
  UnMix(b0, b1, 58);
  UnMix(b2, b3, 22);
  UnMix(b0, b3, 46);
  UnMix(b2, b1, 12);
  UnMix(b0, b1, 25, k2, k3 + t2);
  UnMix(b2, b3, 33, k4 + t0, k0 + 17);
  UnMix(b0, b3, 5);
  UnMix(b2, b1, 37);
  UnMix(b0, b1, 23);
  UnMix(b2, b3, 40);
  UnMix(b0, b3, 52);
  UnMix(b2, b1, 57);
  UnMix(b0, b1, 14, k1, k2 + t1);
  UnMix(b2, b3, 16, k3 + t2, k4 + 16);
  UnMix(b0, b3, 32);
  UnMix(b2, b1, 32);
  UnMix(b0, b1, 58);
  UnMix(b2, b3, 22);
  UnMix(b0, b3, 46);
  UnMix(b2, b1, 12);
  UnMix(b0, b1, 25, k0, k1 + t0);
  UnMix(b2, b3, 33, k2 + t1, k3 + 15);
  UnMix(b0, b3, 5);
  UnMix(b2, b1, 37);
  UnMix(b0, b1, 23);
  UnMix(b2, b3, 40);
  UnMix(b0, b3, 52);
  UnMix(b2, b1, 57);
  UnMix(b0, b1, 14, k4, k0 + t2);
  UnMix(b2, b3, 16, k1 + t0, k2 + 14);
  UnMix(b0, b3, 32);
  UnMix(b2, b1, 32);
  UnMix(b0, b1, 58);
  UnMix(b2, b3, 22);
  UnMix(b0, b3, 46);
  UnMix(b2, b1, 12);
  UnMix(b0, b1, 25, k3, k4 + t1);
  UnMix(b2, b3, 33, k0 + t2, k1 + 13);
  UnMix(b0, b3, 5);
  UnMix(b2, b1, 37);
  UnMix(b0, b1, 23);
  UnMix(b2, b3, 40);
  UnMix(b0, b3, 52);
  UnMix(b2, b1, 57);
  UnMix(b0, b1, 14, k2, k3 + t0);
  UnMix(b2, b3, 16, k4 + t1, k0 + 12);
  UnMix(b0, b3, 32);
  UnMix(b2, b1, 32);
  UnMix(b0, b1, 58);
  UnMix(b2, b3, 22);
  UnMix(b0, b3, 46);
  UnMix(b2, b1, 12);
  UnMix(b0, b1, 25, k1, k2 + t2);
  UnMix(b2, b3, 33, k3 + t0, k4 + 11);
  UnMix(b0, b3, 5);
  UnMix(b2, b1, 37);
  UnMix(b0, b1, 23);
  UnMix(b2, b3, 40);
  UnMix(b0, b3, 52);
  UnMix(b2, b1, 57);
  UnMix(b0, b1, 14, k0, k1 + t1);
  UnMix(b2, b3, 16, k2 + t2, k3 + 10);
  UnMix(b0, b3, 32);
  UnMix(b2, b1, 32);
  UnMix(b0, b1, 58);
  UnMix(b2, b3, 22);
  UnMix(b0, b3, 46);
  UnMix(b2, b1, 12);
  UnMix(b0, b1, 25, k4, k0 + t0);
  UnMix(b2, b3, 33, k1 + t1, k2 + 9);
  UnMix(b0, b3, 5);
  UnMix(b2, b1, 37);
  UnMix(b0, b1, 23);
  UnMix(b2, b3, 40);
  UnMix(b0, b3, 52);
  UnMix(b2, b1, 57);
  UnMix(b0, b1, 14, k3, k4 + t2);
  UnMix(b2, b3, 16, k0 + t0, k1 + 8);
  UnMix(b0, b3, 32);
  UnMix(b2, b1, 32);
  UnMix(b0, b1, 58);
  UnMix(b2, b3, 22);
  UnMix(b0, b3, 46);
  UnMix(b2, b1, 12);
  UnMix(b0, b1, 25, k2, k3 + t1);
  UnMix(b2, b3, 33, k4 + t2, k0 + 7);
  UnMix(b0, b3, 5);
  UnMix(b2, b1, 37);
  UnMix(b0, b1, 23);
  UnMix(b2, b3, 40);
  UnMix(b0, b3, 52);
  UnMix(b2, b1, 57);
  UnMix(b0, b1, 14, k1, k2 + t0);
  UnMix(b2, b3, 16, k3 + t1, k4 + 6);
  UnMix(b0, b3, 32);
  UnMix(b2, b1, 32);
  UnMix(b0, b1, 58);
  UnMix(b2, b3, 22);
  UnMix(b0, b3, 46);
  UnMix(b2, b1, 12);
  UnMix(b0, b1, 25, k0, k1 + t2);
  UnMix(b2, b3, 33, k2 + t0, k3 + 5);
  UnMix(b0, b3, 5);
  UnMix(b2, b1, 37);
  UnMix(b0, b1, 23);
  UnMix(b2, b3, 40);
  UnMix(b0, b3, 52);
  UnMix(b2, b1, 57);
  UnMix(b0, b1, 14, k4, k0 + t1);
  UnMix(b2, b3, 16, k1 + t2, k2 + 4);
  UnMix(b0, b3, 32);
  UnMix(b2, b1, 32);
  UnMix(b0, b1, 58);
  UnMix(b2, b3, 22);
  UnMix(b0, b3, 46);
  UnMix(b2, b1, 12);
  UnMix(b0, b1, 25, k3, k4 + t0);
  UnMix(b2, b3, 33, k0 + t1, k1 + 3);
  UnMix(b0, b3, 5);
  UnMix(b2, b1, 37);
  UnMix(b0, b1, 23);
  UnMix(b2, b3, 40);
  UnMix(b0, b3, 52);
  UnMix(b2, b1, 57);
  UnMix(b0, b1, 14, k2, k3 + t2);
  UnMix(b2, b3, 16, k4 + t0, k0 + 2);
  UnMix(b0, b3, 32);
  UnMix(b2, b1, 32);
  UnMix(b0, b1, 58);
  UnMix(b2, b3, 22);
  UnMix(b0, b3, 46);
  UnMix(b2, b1, 12);
  UnMix(b0, b1, 25, k1, k2 + t1);
  UnMix(b2, b3, 33, k3 + t2, k4 + 1);
  UnMix(b0, b3, 5);
  UnMix(b2, b1, 37);
  UnMix(b0, b1, 23);
  UnMix(b2, b3, 40);
  UnMix(b0, b3, 52);
  UnMix(b2, b1, 57);
  UnMix(b0, b1, 14, k0, k1 + t0);
  UnMix(b2, b3, 16, k2 + t1, k3);

  output[0] = b0;
  output[1] = b1;
  output[2] = b2;
  output[3] = b3;
}

uint8_t Threefish256::GetKeySize()
{
  return (CipherSize/8);
}

uint8_t Threefish256::GetTweakSize()
{
  return 16;
}

uint8_t Threefish256::GetBlockSize()
{
  return GetKeySize();
}

static void WriteUInt8ToUInt64(const uint8_t * const input, uint8_t inputSz, uint64_t * const output)
{
  uint8_t outputPos=0;
  for(uint8_t i=0; i<inputSz; i+=8)
  {
    *(output+outputPos)=static_cast<uint64_t>(input[i])|static_cast<uint64_t>(input[i+1])<<8|static_cast<uint64_t>(input[i+2])<<16|static_cast<uint64_t>(input[i+3])<<24|static_cast<uint64_t>(input[i+4])<<32|static_cast<uint64_t>(input[i+5])<<40|static_cast<uint64_t>(input[i+6])<<48|static_cast<uint64_t>(input[i+7])<<56;
    outputPos++;
  }
}

static void WriteUInt64ToUInt8(const uint64_t * const input, uint8_t input64Sz, uint8_t * const output)
{
  uint8_t outputPos=0;
  for(uint8_t i=0; i<input64Sz; ++i)
  {
    output[outputPos++]=(input[i]&0xFF);
    output[outputPos++]=((input[i]>>8)&0xFF);
    output[outputPos++]=((input[i]>>16)&0xFF);
    output[outputPos++]=((input[i]>>24)&0xFF);
    output[outputPos++]=((input[i]>>32)&0xFF);
    output[outputPos++]=((input[i]>>40)&0xFF);
    output[outputPos++]=((input[i]>>48)&0xFF);
    output[outputPos++]=((input[i]>>56)&0xFF);
  }
}

void Threefish256::SetKey(const uint8_t * const newKey)
{
  uint64_t key[CipherSize/64];
  uint8_t keyBSz=GetKeySize();
  WriteUInt8ToUInt64(newKey,keyBSz,key);
  SetKey(key);
}

void Threefish256::SetTweak(const uint8_t * const newTweak)
{
  uint8_t tweakBSz=GetTweakSize();
  uint64_t tweak[tweakBSz/8];
  WriteUInt8ToUInt64(newTweak,tweakBSz,tweak);
  SetTweak(tweak);
}

void Threefish256::EncryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock)
{
  uint8_t blockSz=GetBlockSize();
  uint8_t block64Sz=blockSz/8;
  uint64_t input[block64Sz];
  uint64_t output[block64Sz];
  WriteUInt8ToUInt64(inputBlock,blockSz,input);
  Encrypt(input,output);
  WriteUInt64ToUInt8(output,block64Sz,outputBlock);
}

void Threefish256::DecryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock)
{
  uint8_t blockSz=GetBlockSize();
  uint8_t block64Sz=blockSz/8;
  uint64_t input[block64Sz];
  uint64_t output[block64Sz];
  WriteUInt8ToUInt64(inputBlock,blockSz,input);
  Decrypt(input,output);
  WriteUInt64ToUInt8(output,block64Sz,outputBlock);
}
