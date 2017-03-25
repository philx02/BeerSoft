#pragma once

#include <array>

#ifdef WIN32

class SpiDevice
{
public:
  SpiDevice(const char *iPath, uint32_t iSpeedHz)
  {
  }

  template< typename T, size_t Size >
  void transfer(const std::array< T, Size > &iTx, std::array< T, Size > &oRx) const
  {
  }
};

#else

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

class SpiDevice
{
public:
  SpiDevice(const char *iPath, uint32_t iSpeedHz)
    : mDevice(open(iPath, O_RDWR))
    , mSpeed(iSpeedHz)
  {
    if (mDevice < 0)
    {
      throw std::runtime_error(std::string("Cannot open device ").append(iPath));
    }
    if (ioctl(mDevice, SPI_IOC_WR_MAX_SPEED_HZ, &iSpeedHz) == -1)
    {
      throw std::runtime_error(std::string("Cannot set SPI_IOC_WR_MAX_SPEED_HZ for ").append(iPath));
    }
    if (ioctl(mDevice, SPI_IOC_RD_MAX_SPEED_HZ, &iSpeedHz) == -1)
    {
      throw std::runtime_error(std::string("Cannot set SPI_IOC_WR_MAX_SPEED_HZ for ").append(iPath));
    }
  }

  template< typename T, size_t Size >
  void transfer(const std::array< T, Size > &iTx, std::array< T, Size > &oRx) const
  {
    struct spi_ioc_transfer wTransfer =
    {
      static_cast< uint64_t >(reinterpret_cast< uintptr_t >(iTx.data())),
      static_cast< uint64_t >(reinterpret_cast< uintptr_t >(oRx.data())),
      Size * sizeof(T),
      mSpeed,
      0,
      8
    };
    if (ioctl(mDevice, SPI_IOC_MESSAGE(1), &wTransfer) == -1)
    {
      throw std::runtime_error("Cannot send SPI message");
    }
  }

private:
  int mDevice;
  uint32_t mSpeed;
};

#endif

class Max6675
{
public:
  Max6675(const char *iPath, int iSpeedHz)
    : mDevice(iPath, iSpeedHz)
  {
  }

  float read() const
  {
    static const std::array< char, 2 > wTxBuffer = { 0, 0 };
    std::array< char, 2 > wRxBuffer;
    mDevice.transfer(wTxBuffer, wRxBuffer);
    std::swap(wRxBuffer[0], wRxBuffer[1]);
    auto &&wValue = reinterpret_cast< uint16_t & >(wRxBuffer);
    wValue >>= 3;
    wValue &= 0xfff;
    float wIntegral = static_cast< float >(wValue >> 2);
    float wDecimal = static_cast< float >(wValue & 0x3) / 4;
    return wIntegral + wDecimal;
  }

private:
  SpiDevice mDevice;
};
