#ifndef AES128_H
#define AES128_H

#include <Arduino.h>
#include <aes.h>
#include "cipher.h"

class AES128 final : public CipherBase
{
  private:
    uint8_t iv[16]{};
    aes_128_context_t context;
  public:
    AES128();
    uint8_t GetKeySize() final;
    uint8_t GetTweakSize() final;
    uint8_t GetBlockSize() final;
    void SetKey(const uint8_t * const newKey) final;
    void SetTweak(const uint8_t * const newTweak) final;
    void EncryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock) final;
    void DecryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock) final;
};

#endif
