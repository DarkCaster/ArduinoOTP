#include "debug.h"
#include "configuration.h"

#ifdef DEBUG

// https://playground.arduino.cc/Code/AvailableMemory

extern unsigned int __heap_start;
extern void *__brkval;

struct Freelist
{
  size_t sz;
  struct Freelist *nx;
};

extern struct Freelist *__flp;

uint32_t get_free_list_size()
{
  struct Freelist* current;
  uint32_t total = 0;
  for (current = __flp; current; current = current->nx)
  {
    total += 2; /* Add two bytes for the memory block's header  */
    total += static_cast<uint32_t>(current->sz);
  }
  return total;
}

uint32_t get_free_memory()
{
  uint32_t free_memory;
  if (__brkval == nullptr)
    free_memory = (reinterpret_cast<uint32_t>(&free_memory)) - (reinterpret_cast<uint32_t>(&__heap_start));
  else
  {
    free_memory = (reinterpret_cast<uint32_t>(&free_memory)) - (reinterpret_cast<uint32_t>(__brkval));
    free_memory += get_free_list_size();
  }
  return free_memory;
}

static bool debug_serial_init_done=false;

#define SERIAL_CHECK (&DEBUG_SERIAL_PORT!=&SERIAL_PORT)

void print_header()
{
  if(!debug_serial_init_done && SERIAL_CHECK)
    DEBUG_SERIAL_PORT.begin(SERIAL_PORT_SPEED);
  debug_serial_init_done=true;
  auto milliseconds=millis();
  auto seconds=milliseconds/1000UL;
  DEBUG_SERIAL_PORT.print(F("["));
  DEBUG_SERIAL_PORT.print(seconds);
  DEBUG_SERIAL_PORT.print(F("s+"));
  DEBUG_SERIAL_PORT.print(milliseconds%(seconds*1000UL));
  DEBUG_SERIAL_PORT.print(F("ms "));
  DEBUG_SERIAL_PORT.print(get_free_memory());
  DEBUG_SERIAL_PORT.print(F("free]: "));
}

void LOG(const __FlashStringHelper* message, const bool nl)
{
  print_header();
  DEBUG_SERIAL_PORT.print(message);
  if(nl)
    DEBUG_SERIAL_PORT.print(F("\n"));
}

#endif
