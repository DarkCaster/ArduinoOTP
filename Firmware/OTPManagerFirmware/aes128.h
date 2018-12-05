#ifndef AES128_H
#define AES128_H

#include <Arduino.h>
#include <aes.h>
#include "cipher.h"

class AES128 final : public CipherBase
{
  public:
    uint8_t GetKeySize() final;
    uint8_t GetTweakSize() final;
    uint8_t GetBlockSize() final;
		void EncryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock, const uint8_t * const key, const uint8_t * const tweak) final;
		void DecryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock, const uint8_t * const key, const uint8_t * const tweak) final;
};

#endif
