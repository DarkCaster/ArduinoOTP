#include "standalone_config.h"
#include "main_loop.h"

#if BUILD_PLATFORM==WINDOWS

#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <csignal>
#include <cstdio>
#include <chrono>
#include <thread>

static volatile boolean hangup=false;

BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
  switch(CEvent)
  {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
      hangup=true;
      printf("Hangup...\n");
      break;
    default:
      break;
  }
  return TRUE;
}

#endif //BUILD_PLATFORM==WINDOWS

int main()
{
#if BUILD_PLATFORM==WINDOWS
  if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,TRUE) == FALSE)
  {
    printf("Unable to install handler!\n");
    return -1;
  }
  printf("Running setup\n");
  setup();
  printf("Starting main loop\n");
  fflush(stdout);
  while(!hangup)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    loop();
  }
  printf("Exit\n");
#else //BUILD_PLATFORM==WINDOWS
#error This platform is not supported for now
#endif
  return 0;
}
