#include <Arduino.h>

#ifndef CIPHER_H
#define CIPHER_H

//NOTE: derived class should be immutable and threadsafe to support sharing it's instance between other objects
class CipherBase
{
  public:
    virtual uint8_t GetKeySize() = 0;
    virtual uint8_t GetTweakSize() = 0;
    virtual uint8_t GetBlockSize() = 0;
		virtual void EncryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock, const uint8_t * const key, const uint8_t * const tweak) = 0;
		virtual void DecryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock, const uint8_t * const key, const uint8_t * const tweak) = 0;
		virtual void MixTweak(const uint8_t * const encData, uint8_t * const curTweak) = 0;
};

#endif
