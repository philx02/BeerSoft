#pragma once

#include <I2CDevice>

class Ads1115
{
public:
  Ads1115(const char *iBus, uint8_t iGainCode = 0, uint8_t iSlaveAddress = 0x48)
    : mDevice(iBus, iSlaveAddress)
    , mCommand(0x8381 | (iGainCode << 1) & 0xE)
  {
  }

  uint16_t read()
  {
    mDevice.writeWord(0x01, mCommand);
    uint16_t wResult = mDevice.readWord(0x00);
    // fix endianess
    return wResult >> 8 | wResult << 8;
  }

private:
  I2CDevice mDevice;
  uint16_t mCommand;
};
