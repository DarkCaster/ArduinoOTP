#include "aes128.h"

#define KEYSZ 16
#define IVSZ 16
#define BLKSZ 16

uint8_t AES128::GetKeySize() { return KEYSZ; }

uint8_t AES128::GetTweakSize() { return IVSZ; }

uint8_t AES128::GetBlockSize() { return BLKSZ; }

void AES128::MixIV(uint8_t* const inputBlock, const uint8_t* const iv)
{
#if BLKSZ != IVSZ
#error BLKSZ != IVSZ
#endif
	//NOTE: check for BLKSZ and IVSZ sizes when porting to use of another algo
	for (uint8_t i = 0; i < IVSZ; ++i)
		 *(inputBlock+i) ^= *(iv+i);
}

void AES128::EncryptBlock(const uint8_t* const inputBlock, uint8_t* const outputBlock, const uint8_t* const key, const uint8_t* const tweak)
{
	aes_128_context_t context;
	aes_128_init(&context, key);
	uint8_t tmpBlock[BLKSZ];
	memcpy(tmpBlock,inputBlock,BLKSZ);
	MixIV(tmpBlock,tweak);
	aes_128_encrypt(&context, tmpBlock);
	memcpy(outputBlock,tmpBlock,BLKSZ);
}

void AES128::DecryptBlock(const uint8_t* const inputBlock, uint8_t* const outputBlock, const uint8_t* const key, const uint8_t* const tweak)
{
	aes_128_context_t context;
	aes_128_init(&context, key);
	uint8_t tmpBlock[BLKSZ];
	memcpy(tmpBlock,inputBlock,BLKSZ);
	aes_128_decrypt(&context, tmpBlock);
	MixIV(tmpBlock,tweak);
	memcpy(outputBlock,tmpBlock,BLKSZ);
}

void AES128::MixTweak(const uint8_t* const encData, uint8_t* const curTweak)
{
#if BLKSZ != IVSZ
#error BLKSZ != IVSZ
#endif
	for(uint8_t tp=0; tp<IVSZ; ++tp)
		*(curTweak+tp)^=*(encData+tp);
}
