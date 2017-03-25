#pragma once

#include <I2CDevice>

class Ads1115
{
public:
  enum class eGainCode
  {
    XFS_6V144,
    X1_4V096,
    X2_2V048,
    X4_1V024,
    X8_0V512,
    X16_0V256
  };

  Ads1115(const char *iBus, eGainCode iGainCode = eGainCode::XFS_6V144, uint8_t iSlaveAddress = 0x48)
    : mDevice(iBus, iSlaveAddress)
    , mCommand(0x83B1 | (static_cast< uint8_t >(iGainCode) << 1) & 0xE)
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
