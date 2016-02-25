#pragma once

#include <I2CDevice>

class Ads1115
{
public:
  Ads1115(const char *iBus, uint8_t iSlaveAddress = 0x48)
    : mDevice(iBus, iSlaveAddress)
  {
  }

  uint16_t read() const
  {
    mDevice.writeWord(0x01, 0x838B);
    uint16_t wResult = mDevice.readWord(0x00);
    // fix endianess
    return wResult >> 8 | wResult << 8;
  }

private:
  I2CDevice mDevice;
};
