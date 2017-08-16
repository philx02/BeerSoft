#include "Ads1115.h"
#include "KegeratorSamplingTasks.h"

class Co2MassSampling
{
public:
  Co2MassSampling(const char *iDevicePath)
    : mAds1115(iDevicePath)
  {
    KegeratorSamplingTasks::getInstance().addSamplingTask([&](DataActiveObject< KegeratorMetrics > &iKegeratorMetrics)
    {
      auto wCo2MassIndex = sampleCo2MassIndex(mAds1115);
      iKegeratorMetrics.pushDataAccess([&](KegeratorMetrics &iMetrics)
      {
        iMetrics.setCo2MassIndex(wCo2MassIndex);
      });
    });
  }

  size_t sampleCo2MassIndex(const Ads1115 &iAds1115)
  {
    auto wValue = iAds1115.read();
    return wValue < 0 ? 0 : wValue;
  }

private:
  Ads1115 mAds1115;
};

static Co2MassSampling sCo2MassSampling(std::getenv("I2C_BUS_PATH"));
