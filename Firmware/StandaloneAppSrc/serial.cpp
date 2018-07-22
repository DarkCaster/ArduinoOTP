#include <cstddef>
#include <cstdint>
#include <cstdio>
#include "serial.h"

HardwareSerial Serial;

#if BUILD_PLATFORM==WINDOWS

void HardwareSerial::begin(unsigned long baud)
{
  hComm = CreateFile(COMPORTNAME, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
  if (hComm == INVALID_HANDLE_VALUE)
  {
    printf("Error in opening serial port\n");
    return;
  }
  printf("Serial port opened\n");
  fflush(stdout);
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  if(!GetCommState(hComm, &dcbSerialParams))
  {
    printf("Error getting comm state: %lu\n",GetLastError());
    return;
  }
  dcbSerialParams.BaudRate = baud;
  dcbSerialParams.ByteSize = 8;
  dcbSerialParams.Parity   = NOPARITY;
  dcbSerialParams.StopBits = ONESTOPBIT;
  if(!SetCommState(hComm, &dcbSerialParams))
  {
    printf("Error setting comm state: %lu\n",GetLastError());
    return;
  }
  if(GetCommTimeouts(hComm, &commTimeout))
  {
    commTimeout.ReadIntervalTimeout = MAXDWORD;
    commTimeout.ReadTotalTimeoutConstant = 0;
    commTimeout.ReadTotalTimeoutMultiplier = 0;
    commTimeout.WriteTotalTimeoutConstant = 0;
    commTimeout.WriteTotalTimeoutMultiplier = 0;
  }
  else
  {
    printf("Error getting timeouts\n");
    return;
  }
  if(!SetCommTimeouts(hComm, &commTimeout))
  {
    printf("Error setting timeouts\n");
    return;
  }
  printf("Serial port config ok\n");
}

void HardwareSerial::end()
{
  CloseHandle(hComm);
}

int HardwareSerial::read(void)
{
  BYTE rx;
  DWORD dwBytesTransferred=0;
  if(ReadFile(hComm, &rx,1, &dwBytesTransferred, 0) && dwBytesTransferred ==1)
    return rx;
  return -1;
}

size_t HardwareSerial::readBytes(char *buffer, size_t length)
{
  if(length<=0)
    return 0;
  int readCNT=0;
  int rx=read();
  while(rx>0 && length>0)
  {
    *(buffer+readCNT)=(BYTE)rx;
    readCNT++;
    length--;
    if(length>0)
      rx=read();
  }
  return readCNT;
}

size_t HardwareSerial::write(uint8_t data)
{
  DWORD dwNumberOfBytesWritten=0;
  if(WriteFile(hComm, &data, 1, &dwNumberOfBytesWritten, 0) && dwNumberOfBytesWritten==1)
    return 1;
  return 0;
}

size_t HardwareSerial::write(char *buffer, size_t length)
{
  if(length==0)
    return 0;
  int pos=0;
  int tx=write(*(buffer+pos));
  while(tx>0 && length>0)
  {
    pos++;
    length--;
    if(length>0)
      tx=write(*(buffer+pos));
  }
  return pos;
}

int HardwareSerial::available(void)
{
  COMSTAT comStat;
  DWORD   dwErrors;
  // Get and clear current errors on the port.
  if (!ClearCommError(hComm, &dwErrors, &comStat))
    return 0;
  if (comStat.cbInQue)
    return comStat.cbInQue;
  return 0;
}

#else
#error platform is not supported for now!
#endif
