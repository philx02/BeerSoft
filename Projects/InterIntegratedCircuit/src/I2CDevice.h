#pragma once

#include "DllSwitch.h"

#include <memory>
#include <array>
#include <stdexcept>
#include <string>

class INTERINTEGRATEDCIRCUIT_LIB I2CDevice
{
public:
  I2CDevice(const char *iDeviceName, int iSlaveAddress);
  ~I2CDevice();

  uint8_t readByte() const;
  uint8_t readByte(uint8_t iAddress) const;
  uint16_t readWord(uint8_t iAddress) const;
  uint16_t readWord() const;
  void writeByte(uint8_t iValue) const;
  void writeByte(uint8_t iAddress, uint8_t iValue) const;
  void writeWord(uint8_t iAddress, uint16_t iValue) const;

  class CannotOpenDevice : public std::runtime_error
  {
  public:
    CannotOpenDevice(const char *iWhat) : std::runtime_error(iWhat) {}
  };
  class SlaveNotFound : public std::runtime_error
  {
  public:
    SlaveNotFound(int iSlaveAddress) : mWhat(std::to_string(iSlaveAddress)), std::runtime_error(mWhat.c_str()) {}
  private:
    std::string mWhat;
  };

private:
  class Impl;
  #pragma warning( push )
  #pragma warning( disable : 4251 )
  std::unique_ptr< Impl > mImpl;
  #pragma warning( pop )
};
