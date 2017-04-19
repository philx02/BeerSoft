#pragma once

#include "Subject.h"
#include "FilteredValue.h"
#include "MovingAveragedValue.h"
#include "Statement.h"

#include "sqlite/sqlite3.h"

#include <boost/filesystem.hpp>

#include <string>
#include <atomic>
#include <array>

namespace bfs = boost::filesystem;

class KegeratorMetrics : public Subject< KegeratorMetrics >
{
public:
  KegeratorMetrics(const bfs::path &iDataFolder);

  KegeratorMetrics(KegeratorMetrics &&iKegeratorMetrics);

  void setTemperature(double iTemperature);
  
  void setAmbientPressure(double iPressure);

  void setCo2MassIndex(size_t iCo2MassIndex);

  void pulseKeg(size_t iKegIndex);

  void setKegLevel(size_t iKegIndex, double iLevel);

  void notifyAll() const;

  const static size_t NUMBER_OF_KEGS = 2;

  struct Data
  {
    Data();
    Data(const Data &iData);

    FilteredValue mTemperature;
    FilteredValue mAmbientPressure;
    MovingAveragedValue mCo2MassIndex;
    struct PulsesInfo
    {
      PulsesInfo();
      mutable std::atomic< int > mPulsesFlushCountdown;
      std::atomic< size_t > mPulses;
    };
    std::array< PulsesInfo, NUMBER_OF_KEGS > mKegsActualPulses;
  };

  const Data &getData() const;

  const std::string dataString() const;

private:
  Data mData;

  size_t mCo2TankEmptyMassIndex;
  size_t mCo2TankFullMassIndex;
  size_t mFullKegTotalPulses;
  sqlite3 *mSqlite;
  std::unique_ptr< Statement > mUpdateKegLevel;
};
