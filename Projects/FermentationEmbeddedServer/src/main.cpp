#include "Mcp3424.h"

#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
  Mcp3424 wMcp3424(getenv("MCP3424_DEVICE_PATH"));
  
  while (true)
  {
    int16_t wRawValue = wMcp3424.read();
    std::cout << wRawValue << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}