#include <cstdio>
#include <cstdint>
#include "gui.h"

#define LOG(...) ({printf(__VA_ARGS__); printf("\n"); fflush(stdout);})

void GuiU8G2::Init(uint8_t addr)
{
  LOG("GuiU8G2::Init: address 0x%02x", addr);
}
