#include "Bmp180.h"
#include "KegeratorSamplingTasks.h"

class PressureAndTemperatureSampling
{
public:
  PressureAndTemperatureSampling(const char *iDevicePath)
    : mBmp180(iDevicePath)
  {
    KegeratorSamplingTasks::getInstance().addSamplingTask([&](DataActiveObject< KegeratorMetrics > &iKegeratorMetrics)
    {
      auto wData = samplePressureAndTemperature(mBmp180);
      iKegeratorMetrics.pushDataAccess([&](KegeratorMetrics &iMetrics)
      {
        iMetrics.setAmbientPressure(wData.mPressure);
        iMetrics.setTemperature(wData.mTemperature);
      });
    });
  }
  
  struct PressureTemperature
  {
    PressureTemperature(double iPressure, double iTemperature)
      : mPressure(iPressure)
      , mTemperature(iTemperature)
    {
    }
    double mPressure;
    double mTemperature;
  };

  PressureTemperature samplePressureAndTemperature(const Bmp180 &iBmp180)
  {
    auto wData = iBmp180.getTemperature();
    return PressureTemperature(iBmp180.getPressure(wData.mPressureFactor), wData.mTemperature);
  }

private:
  Bmp180 mBmp180;
};

static PressureAndTemperatureSampling sPressureAndTemperatureSampling(std::getenv("I2C_BUS_PATH"));
