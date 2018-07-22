#include <cstdio>
#include <cstdarg>
#include <chrono>
#include "arduino-defines.h"

#pragma GCC diagnostic ignored "-Wunused-function"

void LOG(const char * message, ... )
{
  va_list args;
  va_start(args, message);
  vprintf (message, args);
  printf("\n");
  fflush(stdout);
  va_end(args);
}

unsigned long millis()
{
  return static_cast<unsigned long>((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())).count());
}

#pragma GCC diagnostic pop
