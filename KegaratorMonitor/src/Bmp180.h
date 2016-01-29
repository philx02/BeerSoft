#pragma once

#include <I2CDevice>

#include <cstdint>
#include <tuple>
#include <thread>

class Bmp180
{
public:
  Bmp180(const char *iBus)
    : mDevice(iBus, 0x77)
    , ac1((mDevice.readByte(0xAA) << 8) | mDevice.readByte(0xAB))
    , ac2((mDevice.readByte(0xAC) << 8) | mDevice.readByte(0xAD))
    , ac3((mDevice.readByte(0xAE) << 8) | mDevice.readByte(0xAF))
    , ac4((mDevice.readByte(0xB0) << 8) | mDevice.readByte(0xB1))
    , ac5((mDevice.readByte(0xB2) << 8) | mDevice.readByte(0xB3))
    , ac6((mDevice.readByte(0xB4) << 8) | mDevice.readByte(0xB5))
    , b1((mDevice.readByte(0xB6) << 8) | mDevice.readByte(0xB7))
    , b2((mDevice.readByte(0xB8) << 8) | mDevice.readByte(0xB9))
    , mb((mDevice.readByte(0xBA) << 8) | mDevice.readByte(0xBB))
    , mc((mDevice.readByte(0xBC) << 8) | mDevice.readByte(0xBD))
    , md((mDevice.readByte(0xBE) << 8) | mDevice.readByte(0xBF))
    , mOss(0)
  {
  }
  
  std::tuple< double, int32_t > getTemperature()
  {
    uint16_t ut = readUT();

    int32_t x1, x2, b5;
  
    x1 = ((static_cast< int32_t >(ut) - static_cast< int32_t >(ac6))*static_cast< int32_t >(ac5)) >> 15;
    int32_t wDivisor = (x1 + md);
    x2 = wDivisor != 0 ? (static_cast< int32_t >(mc) << 11)/wDivisor : 0;
    b5 = x1 + x2;

    return std::make_tuple(static_cast< double >((b5 + 8) >> 4) / 10.0, b5);
  }
  
  double getPressure(int32_t b5)
  {
    uint32_t up = readUP();
    int32_t x1, x2, x3, b3, b6, p;
    uint32_t b4, b7;
  
    b6 = b5 - 4000;
    // Calculate B3
    x1 = (b2 * (b6 * b6)>>12)>>11;
    x2 = (ac2 * b6)>>11;
    x3 = x1 + x2;
    b3 = ((((static_cast< int32_t >(ac1))*4 + x3)<<mOss) + 2)>>2;
  
    // Calculate B4
    x1 = (ac3 * b6)>>13;
    x2 = (b1 * ((b6 * b6)>>12))>>16;
    x3 = ((x1 + x2) + 2)>>2;
    b4 = (ac4 * static_cast< uint32_t >(x3 + 32768))>>15;
  
    b7 = (static_cast< uint32_t >(up - b3) * (50000>>mOss));
    if (b4 != 0)
    {
      if (b7 < 0x80000000)
        p = (b7<<1)/b4;
      else
        p = (b7/b4)<<1;
    }
    else
    {
      p = 0;
    }
    
    x1 = (p>>8) * (p>>8);
    x1 = (x1 * 3038)>>16;
    x2 = (-7357 * p)>>16;
    p += (x1 + x2 + 3791)>>4;
  
    return static_cast< double >(p);
  }

private:
  uint16_t readUT()
  {
    // Write 0x2E into Register 0xF4
    // This requests a temperature reading
    mDevice.writeByte(0xF4, 0x2E);
  
    // Wait at least 4.5ms
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  
    // Read two bytes from registers 0xF6 and 0xF7
    return (mDevice.readByte(0xF6) << 8) | mDevice.readByte(0xF7);
  }

  uint32_t readUP()
  {
    // Write 0x34+(OSS<<6) into register 0xF4
    // Request a pressure reading w/ oversampling setting
    mDevice.writeByte(0xF4, 0x34 + (mOss << 6));
  
    // Wait for conversion, delay time dependent on OSS
    std::this_thread::sleep_for(std::chrono::milliseconds(2 + (3 << mOss)));
  
    return ((mDevice.readByte(0xF6) << 16) | (mDevice.readByte(0xF7) << 8) | mDevice.readByte(0xF8)) >> (8 - mOss);
  }

	I2CDevice mDevice;
  int16_t ac1;
  int16_t ac2;
  int16_t ac3;
  uint16_t ac4;
  uint16_t ac5;
  uint16_t ac6;
  int16_t b1;
  int16_t b2;
  int16_t mb;
  int16_t mc;
  int16_t md;
  uint8_t mOss;
};
