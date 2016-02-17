#pragma once

#include "Subject.h"
#include <cassert>
#include <string>
#include <atomic>
#include <array>
#include <fstream>

#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

class KegaratorMetrics : public Subject< KegaratorMetrics >
{
public:
  KegaratorMetrics(const bfs::path &iDataFolder)
  {
    auto wRetrievePulses = [&](size_t wKegId, const char *iFilename)
    {
      mKegsActualPulsesFiles[wKegId].open((iDataFolder / iFilename).c_str());
      size_t wRetrievedPulses;
      mKegsActualPulsesFiles[wKegId] >> wRetrievedPulses;
      mData.mKegsActualPulses[wKegId].store(wRetrievedPulses);
    };
    wRetrievePulses(0, "Keg0Pulses.txt");
    wRetrievePulses(1, "Keg1Pulses.txt");
    auto wRetrieveValue = [&](const char *iFilename) -> size_t
    {
      std::ifstream wFile(iFilename);
      size_t wValue;
      wFile >> wValue;
      return wValue;
    };
    mCo2TankEmptyMassIndex = wRetrieveValue("Co2TankEmptyMassIndex.txt");
    mCo2TankFullMassIndex = wRetrieveValue("Co2TankFullMassIndex.txt");
    mFullKegTotalPulses = wRetrieveValue("FullKegTotalPulses.txt");
  }

  KegaratorMetrics(KegaratorMetrics &&iKegaratorMetrics)
    : mData(std::move(iKegaratorMetrics.mData))
    , mCo2TankEmptyMassIndex(std::move(iKegaratorMetrics.mCo2TankEmptyMassIndex))
    , mCo2TankFullMassIndex(std::move(iKegaratorMetrics.mCo2TankFullMassIndex))
    , mFullKegTotalPulses(std::move(iKegaratorMetrics.mFullKegTotalPulses))
  {
    mKegsActualPulsesFiles[0] = std::move(iKegaratorMetrics.mKegsActualPulsesFiles[0]);
    mKegsActualPulsesFiles[1] = std::move(iKegaratorMetrics.mKegsActualPulsesFiles[1]);
  }

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

  void setCo2MassIndex(size_t iCo2MassIndex)
  {
    if (iCo2MassIndex != mData.mCo2MassIndex)
    {
      mData.mCo2MassIndex = iCo2MassIndex;
      Subject< KegaratorMetrics >::notify(*this);
    }
  }

  void pulseKeg(size_t iKegIndex)
  {
    assert(iKegIndex < NUMBER_OF_KEGS);
    ++mData.mKegsActualPulses[iKegIndex];
    Subject< KegaratorMetrics >::notify(*this);
  }

  const static size_t NUMBER_OF_KEGS = 2;

  struct Data
  {
    Data()
      : mTemperature(0)
      , mAmbientPressure(0)
    {
      for (auto &&wKegPulses : mKegsActualPulses)
      {
        wKegPulses = 0;
      }
    }

    Data(const Data &iData)
      : mTemperature(iData.mTemperature.load())
      , mAmbientPressure(iData.mAmbientPressure.load())
    {
      for (auto wKegPulsesIndex = 0u; wKegPulsesIndex < mKegsActualPulses.size(); ++wKegPulsesIndex)
      {
        mKegsActualPulses[wKegPulsesIndex] = iData.mKegsActualPulses[wKegPulsesIndex].load();
      }
    }

    std::atomic< double > mTemperature;
    std::atomic< double > mAmbientPressure;
    std::atomic< size_t > mCo2MassIndex;
    std::array< std::atomic< size_t >, NUMBER_OF_KEGS > mKegsActualPulses;
  };

  const Data &getData() const
  {
    return mData;
  }

  const std::string dataString() const
  {
    auto wCo2Ratio = mData.mCo2MassIndex < mCo2TankFullMassIndex ? static_cast< double >(mData.mCo2MassIndex - mCo2TankEmptyMassIndex) / static_cast< double >(mCo2TankFullMassIndex - mCo2TankEmptyMassIndex) : 1.0;
    auto wKeg0Ratio = mData.mKegsActualPulses[0] < mFullKegTotalPulses ? static_cast< double >(mFullKegTotalPulses - mData.mKegsActualPulses[0]) / static_cast< double >(mFullKegTotalPulses) : 0.0;
    auto wKeg1Ratio = mData.mKegsActualPulses[1] < mFullKegTotalPulses ? static_cast< double >(mFullKegTotalPulses - mData.mKegsActualPulses[1]) / static_cast< double >(mFullKegTotalPulses) : 0.0;
    return    std::to_string(mData.mTemperature)
      + "," + std::to_string(mData.mAmbientPressure)
      + "," + std::to_string(wCo2Ratio)
      + "," + std::to_string(wKeg0Ratio)
      + "," + std::to_string(wKeg1Ratio);
  }

private:
  Data mData;
  size_t mCo2TankEmptyMassIndex;
  size_t mCo2TankFullMassIndex;
  size_t mFullKegTotalPulses;
  std::array< std::fstream, NUMBER_OF_KEGS > mKegsActualPulsesFiles;
};
