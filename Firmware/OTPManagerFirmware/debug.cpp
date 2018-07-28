#include "debug.h"
#include "configuration.h"

#ifdef DEBUG

static bool debug_serial_init_done=false;

#define SERIAL_CHECK (&DEBUG_SERIAL_PORT!=&SERIAL_PORT)

void LOG(const char * message, va_list args)
{
  if(!debug_serial_init_done && SERIAL_CHECK)
    DEBUG_SERIAL_PORT.begin(SERIAL_PORT_SPEED);
  debug_serial_init_done=true;
  char out_buff[DEBUG_OUT_BUFFER_SIZE+1];
  out_buff[DEBUG_OUT_BUFFER_SIZE]=0;
  auto milliseconds=millis();
  auto seconds=milliseconds/1000UL;
  //auto milli_seconds=static_cast<uint16_t>();
  const static char timeFormat[] PROGMEM = "[%03lu.%03lu]: ";
  snprintf_P(out_buff, DEBUG_OUT_BUFFER_SIZE, timeFormat, seconds, milliseconds%(seconds*1000UL));
  DEBUG_SERIAL_PORT.write(out_buff);
  vsnprintf(out_buff, DEBUG_OUT_BUFFER_SIZE, message, args);
  DEBUG_SERIAL_PORT.write(out_buff);
  DEBUG_SERIAL_PORT.write("\n");
}

void LOG(const __FlashStringHelper* message, ... )
{
  auto P_message = reinterpret_cast<PGM_P>(message);
  auto msgSize=strlen_P(P_message);
  char tmp[msgSize+1];
  tmp[msgSize]=0;
  strcpy_P(tmp,P_message);
  va_list args;
  va_start(args, message);
  LOG(tmp,args);
  va_end(args);
}


void LOG(const char * message, ... )
{
  va_list args;
  va_start(args, message);
  LOG(message,args);
  va_end(args);
}

#endif
