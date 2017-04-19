#include "KegeratorMetrics.h"

const static size_t NUMBER_OF_KEGS = 2;

static size_t retrieveEnvVar(const char *iName, size_t iDefaultValue)
{
  auto wMovingAverageWindow = std::getenv(iName);
  return wMovingAverageWindow != nullptr ? std::strtoul(wMovingAverageWindow, nullptr, 10) : iDefaultValue;
}

KegeratorMetrics::KegeratorMetrics(const bfs::path &iDataFolder)
{
  auto wResult = sqlite3_open_v2((iDataFolder / "kegerator.sqlite").string().c_str(), &mSqlite, SQLITE_OPEN_READWRITE, nullptr);
  if (wResult != SQLITE_OK)
  {
    throw std::runtime_error(sqlite3_errstr(wResult));
  }
  Statement(mSqlite, "PRAGMA foreign_keys = ON").runOnce();
  mUpdateKegLevel.reset(new Statement(mSqlite, "UPDATE keg_level SET level = ?1 WHERE id = ?2"));

  auto wRetrievePulses = [&](size_t iKegId)
  {
    Statement wRetrieveKegLevel(mSqlite, std::string("SELECT level FROM keg_level WHERE id = ").append(std::to_string(iKegId)).c_str());
    while (wRetrieveKegLevel.runOnce() == SQLITE_ROW)
    {
      wRetrieveKegLevel.evaluate([&](sqlite3_stmt *iStatement)
      {
        mData.mKegsActualPulses[iKegId].mPulses.store(sqlite3_column_int(iStatement, 0));
      });
    }
  };
  wRetrievePulses(0);
  wRetrievePulses(1);
  auto wRetrieveValue = [&](const char *iName) -> size_t
  {
    Statement wRetrieveKegLevel(mSqlite, std::string("SELECT value FROM parameters WHERE name = '").append(iName).append("'").c_str());
    size_t wValue;
    bool wFound = false;
    while (wRetrieveKegLevel.runOnce() == SQLITE_ROW)
    {
      wFound = true;
      wRetrieveKegLevel.evaluate([&](sqlite3_stmt *iStatement)
      {
        wValue = sqlite3_column_int(iStatement, 0);
      });
    }
    if (!wFound)
    {
      throw std::runtime_error(std::string("Cannot find parameter: ").append(iName));
    }
    return wValue;
  };
  mCo2TankEmptyMassIndex = wRetrieveValue("Co2TankEmptyMassIndex");
  mCo2TankFullMassIndex = wRetrieveValue("Co2TankFullMassIndex");
  mFullKegTotalPulses = wRetrieveValue("FullKegTotalPulses");
}

KegeratorMetrics::KegeratorMetrics(KegeratorMetrics &&iKegeratorMetrics)
  : mData(std::move(iKegeratorMetrics.mData))
  , mCo2TankEmptyMassIndex(std::move(iKegeratorMetrics.mCo2TankEmptyMassIndex))
  , mCo2TankFullMassIndex(std::move(iKegeratorMetrics.mCo2TankFullMassIndex))
  , mFullKegTotalPulses(std::move(iKegeratorMetrics.mFullKegTotalPulses))
  , mUpdateKegLevel(std::move(iKegeratorMetrics.mUpdateKegLevel))
{
}

void KegeratorMetrics::setTemperature(double iTemperature)
{
  mData.mTemperature.addSample(iTemperature);
}
  
void KegeratorMetrics::setAmbientPressure(double iPressure)
{
  mData.mAmbientPressure.addSample(iPressure);
}

void KegeratorMetrics::setCo2MassIndex(size_t iCo2MassIndex)
{
  mData.mCo2MassIndex.addSample(iCo2MassIndex);
}

void KegeratorMetrics::pulseKeg(size_t iKegIndex)
{
  assert(iKegIndex < NUMBER_OF_KEGS);
  ++mData.mKegsActualPulses[iKegIndex].mPulses;
  mData.mKegsActualPulses[iKegIndex].mPulsesFlushCountdown = 5;
}

void KegeratorMetrics::setKegLevel(size_t iKegIndex, double iLevel)
{
  assert(iKegIndex < NUMBER_OF_KEGS);
  mData.mKegsActualPulses[iKegIndex].mPulses = mFullKegTotalPulses - static_cast< decltype(mFullKegTotalPulses) >(iLevel * static_cast< double >(mFullKegTotalPulses));
  mData.mKegsActualPulses[iKegIndex].mPulsesFlushCountdown = 5;
}

void KegeratorMetrics::notifyAll() const
{
  for (size_t wIndex = 0; wIndex < NUMBER_OF_KEGS; ++wIndex)
  {
    if (mData.mKegsActualPulses[wIndex].mPulsesFlushCountdown == 0)
    {
      mUpdateKegLevel->bind(1, mData.mKegsActualPulses[wIndex].mPulses.load());
      mUpdateKegLevel->bind(2, wIndex);
      mUpdateKegLevel->runOnce();
      --mData.mKegsActualPulses[wIndex].mPulsesFlushCountdown;
    }
    else if (mData.mKegsActualPulses[wIndex].mPulsesFlushCountdown >= 0)
    {
      --mData.mKegsActualPulses[wIndex].mPulsesFlushCountdown;
    }
  }

  Subject< KegeratorMetrics >::notify(*this);
}

KegeratorMetrics::Data::Data()
  : mTemperature(0, 0.1)
  , mAmbientPressure(0, 0.1)
  , mCo2MassIndex(retrieveEnvVar("CO2_METER_MOVING_AVERAGE_WINDOW", 120))
{
}

KegeratorMetrics::Data::Data(const Data &iData)
  : mTemperature(iData.mTemperature)
  , mAmbientPressure(iData.mAmbientPressure)
  , mCo2MassIndex(iData.mCo2MassIndex)
{
  for (size_t wIndex = 0; wIndex < NUMBER_OF_KEGS; ++wIndex)
  {
    mKegsActualPulses[wIndex].mPulses = iData.mKegsActualPulses[wIndex].mPulses.load();
    mKegsActualPulses[wIndex].mPulsesFlushCountdown = iData.mKegsActualPulses[wIndex].mPulsesFlushCountdown.load();
  }
}

KegeratorMetrics::Data::PulsesInfo::PulsesInfo()
  : mPulsesFlushCountdown(-1)
  , mPulses(0)
{
}

const KegeratorMetrics::Data &KegeratorMetrics::getData() const
{
  return mData;
}

const std::string KegeratorMetrics::dataString() const
{
  auto wCo2Ratio = [&]() -> double
  {
    auto &&wActualValue = mData.mCo2MassIndex.getActualValue();
    if (wActualValue < mCo2TankEmptyMassIndex)
    {
      return 0.0;
    }
    else if (wActualValue < mCo2TankFullMassIndex)
    {
      return static_cast<double>(wActualValue - mCo2TankEmptyMassIndex) / static_cast<double>(mCo2TankFullMassIndex - mCo2TankEmptyMassIndex);
    }
    else
    {
      return 1.0;
    }
  }();
  auto wKeg0Ratio = mData.mKegsActualPulses[0].mPulses < mFullKegTotalPulses ? static_cast< double >(mFullKegTotalPulses - mData.mKegsActualPulses[0].mPulses) / static_cast< double >(mFullKegTotalPulses) : 0.0;
  auto wKeg1Ratio = mData.mKegsActualPulses[1].mPulses < mFullKegTotalPulses ? static_cast< double >(mFullKegTotalPulses - mData.mKegsActualPulses[1].mPulses) / static_cast< double >(mFullKegTotalPulses) : 0.0;
  return    std::to_string(mData.mTemperature.getActualValue())
    + "," + std::to_string(mData.mAmbientPressure.getActualValue())
    + "," + std::to_string(wCo2Ratio)
    + "," + std::to_string(wKeg0Ratio)
    + "," + std::to_string(wKeg1Ratio);
}
