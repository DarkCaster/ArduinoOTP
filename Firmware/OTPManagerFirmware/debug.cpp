#include "debug.h"
#include "configuration.h"

#if DEBUG

static uint8_t debug_serial_init_done=0;
#define SERIAL_CHECK (&DEBUG_SERIAL_PORT!=&SERIAL_PORT)
void LOG(const char * message, ... )
{
  if(!debug_serial_init_done && SERIAL_CHECK)
    DEBUG_SERIAL_PORT.begin(SERIAL_PORT_SPEED);
  debug_serial_init_done=1;
  va_list args;
  va_start(args, message);
  char out_buff[DEBUG_OUT_BUFFER_SIZE+1];
  out_buff[DEBUG_OUT_BUFFER_SIZE]=0;
  vsnprintf(out_buff, DEBUG_OUT_BUFFER_SIZE, message, args);
  va_end(args);
  DEBUG_SERIAL_PORT.write(out_buff);
}

#endif
