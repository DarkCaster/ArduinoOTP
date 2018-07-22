#include <cstdio>
#include <cstdarg>
#include "arduino-defines.h"

#pragma GCC diagnostic ignored "-Wunused-function"

void LOG(const char * message, ... )
{
  va_list args;
  va_start(args, message);
  printf(message, args);
  printf("\n");
  fflush(stdout);
  va_end(args);
}

#pragma GCC diagnostic pop
