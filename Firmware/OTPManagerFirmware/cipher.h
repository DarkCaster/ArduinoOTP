#include <Arduino.h>

#ifndef CIPHER_H
#define CIPHER_H

class CipherBase
{
  public:
    virtual uint8_t GetKeySize() = 0;
    virtual uint8_t GetTweakSize() = 0;
    virtual uint8_t GetBlockSize() = 0;
    virtual void SetKey(const uint8_t * const NewKey) = 0;
    virtual void SetTweak(const uint8_t * const NewTweak) = 0;
    virtual void EncryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock) = 0;
    virtual void DecryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock) = 0;
};

#endif
