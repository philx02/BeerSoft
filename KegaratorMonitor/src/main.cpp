#include "Ads1115.h"
#include "Bmp180.h"

#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
  Ads1115 mAds1115("/dev/i2c-1");
  
  while (true)
  {
    int16_t wRawValue = mAds1115.read();
    std::cout << wRawValue << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}