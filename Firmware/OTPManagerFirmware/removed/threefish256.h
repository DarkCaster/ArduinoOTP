#ifndef THREEFISH256_H
#define THREEFISH256_H

#include <Arduino.h>
#include "cipher.h"

// based on https://github.com/nitrocaster/SkeinFish implementation, MIT-license

class Threefish256 final : public CipherBase
{
  private:
    static const uint64_t KeyScheduleConst = 0x1BD11BDAA9FC1A22;
    static const uint8_t ExpandedTweakSize = 3;
    uint64_t ExpandedTweak[ExpandedTweakSize]{};

    static const uint16_t CipherSize = 256;
    static const uint8_t ExpandedKeySize = (CipherSize / 64) + 1;
    uint64_t ExpandedKey[ExpandedKeySize]{};

    static uint64_t RotateLeft64(uint64_t v, int8_t b);
    static uint64_t RotateRight64(uint64_t v, int8_t b);
    static void Mix(uint64_t &a, uint64_t &b, int8_t r);
    static void Mix(uint64_t &a, uint64_t &b, int8_t r, uint64_t k0, uint64_t k1);
    static void UnMix(uint64_t &a, uint64_t &b, int8_t r);
    static void UnMix(uint64_t &a, uint64_t &b, int8_t r, uint64_t k0, uint64_t k1);

    void Encrypt(const uint64_t * const input, uint64_t * const output);
    void Decrypt(const uint64_t * const input, uint64_t * const output);
    void SetTweak(const uint64_t * const tweak);
    void SetKey(const uint64_t * const key);
  public:
    uint8_t GetKeySize() final;
    uint8_t GetTweakSize() final;
    uint8_t GetBlockSize() final;
    void SetKey(const uint8_t * const newKey) final;
    void SetTweak(const uint8_t * const newTweak) final;
    void EncryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock) final;
    void DecryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock) final;
};

#endif
