#include "I2CDevice.h"

#include "smbus.h"

#ifndef _WIN32
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#else
int read(int, void *, size_t) { return 0; }
#endif

class I2CDevice::Impl
{
public:
  Impl(const char * iDeviceName, int iSlaveAddress)
#ifdef _WIN32
    : mDevice(0)
#else
    : mDevice(open(iDeviceName, O_RDWR))
#endif
  {
#ifndef _WIN32
    if (mDevice >= 0)
    {
      if (ioctl(mDevice, I2C_SLAVE, iSlaveAddress) < 0)
      {
        throw SlaveNotFound(iSlaveAddress);
      }
    }
    else
    {
      throw CannotOpenDevice(iDeviceName);
    }
#endif
  }
  ~Impl()
  {
#ifndef _WIN32
    if (mDevice > 0)
    {
      close(mDevice);
    }
#endif
  }

  int getDevice()
  {
    return mDevice;
  }

private:
  int mDevice;
};

I2CDevice::I2CDevice(const char *iDeviceName, int iSlaveAddress)
  : mImpl(new Impl(iDeviceName, iSlaveAddress))
{
}

I2CDevice::~I2CDevice()
{
}

uint8_t I2CDevice::readByte() const
{
  return i2c_smbus_read_byte(mImpl->getDevice());
}

uint8_t I2CDevice::readByte(uint8_t iAddress) const
{
  //int32_t wResult = i2c_smbus_write_byte(mImpl->getDevice(), iAddress);
  return i2c_smbus_read_byte_data(mImpl->getDevice(), iAddress);
}

uint16_t I2CDevice::readWord(uint8_t iAddress) const
{
  //int32_t wResult = i2c_smbus_write_byte(mImpl->getDevice(), iAddress);
  return i2c_smbus_read_word_data(mImpl->getDevice(), iAddress);
}

uint16_t I2CDevice::readWord() const
{
  uint16_t wBuffer;
  auto wResult = read(mImpl->getDevice(), &wBuffer, sizeof(wBuffer));
  if (wResult < 0)
  {
    return wResult;
  }
  return wBuffer;
}

void I2CDevice::writeByte(uint8_t iValue) const
{
  i2c_smbus_write_byte(mImpl->getDevice(), iValue);
}

void I2CDevice::writeByte(uint8_t iAddress, uint8_t iValue) const
{
	i2c_smbus_write_byte_data(mImpl->getDevice(), iAddress, iValue);
}

void I2CDevice::writeWord(uint8_t iAddress, uint16_t iValue) const
{
  i2c_smbus_write_word_data(mImpl->getDevice(), iAddress, iValue);
}
