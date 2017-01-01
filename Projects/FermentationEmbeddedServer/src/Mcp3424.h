#pragma once

#include <I2CDevice>

class Mcp3424Channel
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

  Mcp3424Channel(std::shared_ptr< I2CDevice > iDevice, uint8_t iChannel = 0, Resolution iResolution = Resolution::bits16, Gain iGain = Gain::x8)
    : mConfigByte(0x10)
    , mDevice(std::move(iDevice))
  {
    mConfigByte |= (iChannel & 0x3) << 5;
    mConfigByte |= (static_cast< uint8_t >(iResolution) & 0x3) << 2;
    mConfigByte |= (static_cast< uint8_t >(iGain) & 0x3);
  }

  uint16_t read() const
  {
    mDevice->writeByte(mConfigByte);
    uint16_t wResult = mDevice->readWord();
    // fix endianess
    return wResult >> 8 | wResult << 8;
  }

private:
  uint8_t mConfigByte;
  std::shared_ptr< I2CDevice > mDevice;
};
