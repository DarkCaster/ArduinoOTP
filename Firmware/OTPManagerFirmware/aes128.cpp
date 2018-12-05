#include "aes128.h"

#define KEYSZ 16
#define IVSZ 16
#define BLKSZ 16

uint8_t AES128::GetKeySize() { return KEYSZ; }

uint8_t AES128::GetTweakSize() { return IVSZ; }

uint8_t AES128::GetBlockSize() { return BLKSZ; }

void AES128::EncryptBlock(const uint8_t* const inputBlock, uint8_t* const outputBlock, const uint8_t* const key, const uint8_t* const tweak)
{
	aes_128_context_t context;
	aes_128_init(&context, key);
	uint8_t tmpBlock[BLKSZ];
	for(uint8_t i=0; i<BLKSZ; ++i)
		*(tmpBlock+i)=*(inputBlock+i);
#if BLKSZ != IVSZ
#error BLKSZ != IVSZ
#endif
	//NOTE: check for BLKSZ and IVSZ sizes when porting to use of another algo
	for (uint8_t i = 0; i < IVSZ; ++i)
		 *(tmpBlock+i) ^= *(tweak+i);
	aes_128_encrypt(&context, tmpBlock);
	for (uint8_t i = 0; i < BLKSZ; ++i)
		 *(outputBlock+i) = *(tmpBlock+i);
}

void AES128::DecryptBlock(const uint8_t* const inputBlock, uint8_t* const outputBlock, const uint8_t* const key, const uint8_t* const tweak)
{
	aes_128_context_t context;
	aes_128_init(&context, key);
	uint8_t tmpBlock[BLKSZ];
	for(uint8_t i=0; i<BLKSZ; ++i)
		*(tmpBlock+i)=*(inputBlock+i);
	aes_128_decrypt(&context, tmpBlock);
#if BLKSZ != IVSZ
#error BLKSZ != IVSZ
#endif
	//NOTE: check for BLKSZ and IVSZ sizes when porting to use of another algo
	for (uint8_t i = 0; i < IVSZ; ++i)
		 *(tmpBlock+i) ^= *(tweak+i);
	for (uint8_t i = 0; i < BLKSZ; ++i)
		 *(outputBlock+i) = *(tmpBlock+i);
}
