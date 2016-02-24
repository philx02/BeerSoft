#pragma once

#include "Subject.h"
#include "FilteredValue.h"

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
      mKegsActualPulsesFiles[wKegId].reset(new std::fstream((iDataFolder / iFilename).c_str()));
      size_t wRetrievedPulses;
      *mKegsActualPulsesFiles[wKegId] >> wRetrievedPulses;
      mData.mKegsActualPulses[wKegId].store(wRetrievedPulses);
    };
    wRetrievePulses(0, "Keg0Pulses.txt");
    wRetrievePulses(1, "Keg1Pulses.txt");
    auto wRetrieveValue = [&](const char *iFilename) -> size_t
    {
      std::ifstream wFile((iDataFolder / iFilename).c_str());
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
    mData.mTemperature.addSample(iTemperature);
    Subject< KegaratorMetrics >::notify(*this);
  }
  
  void setAmbientPressure(double iPressure)
  {
    mData.mAmbientPressure.addSample(iPressure);
    Subject< KegaratorMetrics >::notify(*this);
  }

  void setCo2MassIndex(size_t iCo2MassIndex)
  {
    mData.mCo2MassIndex.addSample(iCo2MassIndex);
    Subject< KegaratorMetrics >::notify(*this);
  }

  void pulseKeg(size_t iKegIndex)
  {
    assert(iKegIndex < NUMBER_OF_KEGS);
    mKegsActualPulsesFiles[iKegIndex]->seekp(0);
    *mKegsActualPulsesFiles[iKegIndex] << ++mData.mKegsActualPulses[iKegIndex];
    mKegsActualPulsesFiles[iKegIndex]->flush();
    Subject< KegaratorMetrics >::notify(*this);
  }

  const static size_t NUMBER_OF_KEGS = 2;

  struct Data
  {
    Data()
      : mTemperature(0, 0.4)
      , mAmbientPressure(0, 0.4)
      , mCo2MassIndex(0, 0.4)
    {
      for (auto &&wKegPulses : mKegsActualPulses)
      {
        wKegPulses = 0;
      }
    }

    Data(const Data &iData)
      : mTemperature(iData.mTemperature)
      , mAmbientPressure(iData.mAmbientPressure)
      , mCo2MassIndex(iData.mCo2MassIndex)
    {
      for (auto wKegPulsesIndex = 0u; wKegPulsesIndex < mKegsActualPulses.size(); ++wKegPulsesIndex)
      {
        mKegsActualPulses[wKegPulsesIndex] = iData.mKegsActualPulses[wKegPulsesIndex].load();
      }
    }

    FilteredValue mTemperature;
    FilteredValue mAmbientPressure;
    FilteredValue mCo2MassIndex;
    std::array< std::atomic< size_t >, NUMBER_OF_KEGS > mKegsActualPulses;
  };

  const Data &getData() const
  {
    return mData;
  }

  const std::string dataString() const
  {
    auto wCo2Ratio = mData.mCo2MassIndex.getActualValue() < mCo2TankFullMassIndex ? static_cast< double >(mData.mCo2MassIndex.getActualValue() - mCo2TankEmptyMassIndex) / static_cast< double >(mCo2TankFullMassIndex - mCo2TankEmptyMassIndex) : 1.0;
    auto wKeg0Ratio = mData.mKegsActualPulses[0] < mFullKegTotalPulses ? static_cast< double >(mFullKegTotalPulses - mData.mKegsActualPulses[0]) / static_cast< double >(mFullKegTotalPulses) : 0.0;
    auto wKeg1Ratio = mData.mKegsActualPulses[1] < mFullKegTotalPulses ? static_cast< double >(mFullKegTotalPulses - mData.mKegsActualPulses[1]) / static_cast< double >(mFullKegTotalPulses) : 0.0;
    return    std::to_string(mData.mTemperature.getActualValue())
      + "," + std::to_string(mData.mAmbientPressure.getActualValue())
      + "," + std::to_string(wCo2Ratio)
      + "," + std::to_string(wKeg0Ratio)
      + "," + std::to_string(wKeg1Ratio);
  }

private:
  Data mData;

  size_t mCo2TankEmptyMassIndex;
  size_t mCo2TankFullMassIndex;
  size_t mFullKegTotalPulses;
  std::array< std::unique_ptr< std::fstream >, NUMBER_OF_KEGS > mKegsActualPulsesFiles;
};
