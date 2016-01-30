#pragma once

#include "Subject.h"

class KegaratorMetrics : public Subject< KegaratorMetrics >
{
public:
  KegaratorMetrics()
    : mTemperature(0)
    , mAmbientPressure(0)
  {
  }

  void setTemperature(double iTemperature)
  {
    if (iTemperature != mTemperature)
    {
      mTemperature = iTemperature;
      Subject< KegaratorMetrics >::notify(*this);
    }
  }
  
  void setAmbientPressure(double iPressure)
  {
    if (iPressure != mAmbientPressure)
    {
      mAmbientPressure = iPressure;
      Subject< KegaratorMetrics >::notify(*this);
    }
  }

private:
  double mTemperature;
  double mAmbientPressure;
};
