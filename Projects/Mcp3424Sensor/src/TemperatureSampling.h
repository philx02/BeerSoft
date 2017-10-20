#include "Mcp3424.h"
#include <vector>

class TemperatureSampling
{
public:
  TemperatureSampling(const char *iDevicePath, const char *iCalibrationFile);

  double sample() const;

private:
  Mcp3424 mMcp3424;
  uint16_t sampleTemperatureIndex(const Mcp3424 &iMcp3424) const;
  std::vector< double > mTemperatureCalibration;
};
