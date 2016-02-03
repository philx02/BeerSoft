#pragma once

#ifndef WIN32
#include <signal.h>
#include <spawn.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <thread>
#include <iostream>

int openGpio(const char *iGpioPath)
{
  return open(iGpioPath, O_RDONLY);
}

pollfd openSwitchListener(int iGpioFd)
{
  pollfd wPollFd;
  wPollFd.fd = iGpioFd;
  wPollFd.events = POLLPRI | POLLERR;
  return wPollFd;
}

bool readGpio(pollfd &ioPollFd, int iGpioFd)
{
  auto wResult = poll(&ioPollFd, 1, -1);
  lseek(iGpioFd, 0, SEEK_SET);
  char wByte;
  read(iGpioFd, &wByte, 1);
  return wByte == '1';
}

#else

typedef int pollfd;

int openGpio(const char *)
{
  return 0;
}

pollfd openSwitchListener(int)
{
  return 0;
}

template< typename T >
bool readGpio(T, int)
{
  static bool wValue = false;
  wValue = !wValue;
  return wValue;
}

#endif