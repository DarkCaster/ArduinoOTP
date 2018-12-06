#ifndef AES128_H
#define AES128_H

#include <Arduino.h>
#include <aes.h>
#include "cipher.h"

class AES128 final : public CipherBase
{
	private:
		static void MixIV(uint8_t * const inputBlock, const uint8_t * const iv);
  public:
    uint8_t GetKeySize() final;
    uint8_t GetTweakSize() final;
    uint8_t GetBlockSize() final;
		void EncryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock, const uint8_t * const key, const uint8_t * const tweak) final;
		void DecryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock, const uint8_t * const key, const uint8_t * const tweak) final;
		void MixTweak(const uint8_t * const encData, uint8_t * const curTweak) final;
};

#endif
