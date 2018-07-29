#include "aes128.h"

#define KEYSZ 16
#define IVSZ 16
#define BLKSZ 16

AES128::AES128()
{
  uint8_t key[16]={0};
  aes_128_init(&context, key);
}


uint8_t AES128::GetKeySize() { return KEYSZ; }

uint8_t AES128::GetTweakSize() { return IVSZ; }

uint8_t AES128::GetBlockSize() { return BLKSZ; }

void AES128::SetKey(const uint8_t * const newKey)
{
  aes_128_init(&context, newKey);
}

void AES128::SetTweak(const uint8_t * const newTweak)
{
  for(uint8_t i=0;i<IVSZ;++i)
    *(iv+i)=*(newTweak+i);
}

void AES128::EncryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock)
{
  uint8_t tmpBlock[BLKSZ];
  for(uint8_t i=0; i<BLKSZ; ++i)
    *(tmpBlock+i)=*(inputBlock+i);
  for (uint8_t i = 0; i < BLKSZ; i++)
     *(tmpBlock+i) ^= *(iv+i);
  aes_128_encrypt(&context, tmpBlock);
  for (uint8_t i = 0; i < BLKSZ; i++)
     *(outputBlock+i) = *(tmpBlock+i);
}

void AES128::DecryptBlock(const uint8_t * const inputBlock, uint8_t * const outputBlock)
{
  uint8_t tmpBlock[BLKSZ];
  for(uint8_t i=0; i<BLKSZ; ++i)
    *(tmpBlock+i)=*(inputBlock+i);
  aes_128_decrypt(&context, tmpBlock);
  for (uint8_t i = 0; i < BLKSZ; i++)
     *(tmpBlock+i) ^= *(iv+i);
  for (uint8_t i = 0; i < BLKSZ; i++)
     *(outputBlock+i) = *(tmpBlock+i);
}
