#pragma once

#include <I2CDevice>

class Mcp3424
{
public:
  enum class Gain : uint8_t
  {
    x1 = 0,
    x2 = 1,
    x4 = 2,
    x8 = 3
  };
  enum class Resolution : uint8_t
  {
    bits12 = 0,
    bits14 = 1,
    bits16 = 2,
    bits18 = 3
  };

  Mcp3424(const char *iBus, uint8_t iChannel = 0, Resolution iResolution = Resolution::bits12, Gain iGain = Gain::x8, uint8_t iSlaveAddress = 0x68)
    : mDevice(iBus, iSlaveAddress)
  {
    uint8_t wConfigByte = 0x10;
    wConfigByte |= (iChannel & 0x3) << 5;
    wConfigByte |= (static_cast< uint8_t >(iResolution) & 0x3) << 2;
    wConfigByte |= (static_cast< uint8_t >(iGain) & 0x3);
    mDevice.writeByte(wConfigByte);
  }

  uint16_t read() const
  {
    uint16_t wResult = mDevice.readWord();
    // fix endianess
    return wResult >> 8 | wResult << 8;
  }

private:
  I2CDevice mDevice;
};
