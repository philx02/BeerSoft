#pragma once

#include "Subject.h"
#include <cassert>

class KegaratorMetrics : public Subject< KegaratorMetrics >
{
public:
  void setTemperature(double iTemperature)
  {
    if (iTemperature != mData.mTemperature)
    {
      mData.mTemperature = iTemperature;
      Subject< KegaratorMetrics >::notify(*this);
    }
  }
  
  void setAmbientPressure(double iPressure)
  {
    if (iPressure != mData.mAmbientPressure)
    {
      mData.mAmbientPressure = iPressure;
      Subject< KegaratorMetrics >::notify(*this);
    }
  }

  void pulseKeg(size_t iKegIndex)
  {
    assert(iKegIndex < NUMBER_OF_KEGS);
    ++mData.mKegsTotalPulses[iKegIndex];
    Subject< KegaratorMetrics >::notify(*this);
  }

  const static size_t NUMBER_OF_KEGS = 2;

  struct Data
  {
    Data()
      : mTemperature(0)
      , mAmbientPressure(0)
      , mKegsTotalPulses(NUMBER_OF_KEGS, 0)
    {
    }
    double mTemperature;
    double mAmbientPressure;
    std::vector< size_t > mKegsTotalPulses;
  };

  const Data &getData() const
  {
    return mData;
  }

private:
  Data mData;
};
