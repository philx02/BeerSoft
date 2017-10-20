#include "TemperatureSampling.h"

#include <fstream>
#include <string>

#include <boost/algorithm/string.hpp>

TemperatureSampling::TemperatureSampling(const char *iDevicePath, const char *iCalibrationFile)
  : mMcp3424(iDevicePath, 3, Mcp3424::Resolution::bits12)
  , mTemperatureCalibration(std::numeric_limits< uint16_t >::max() + 1)
{
  struct Segment
  {
    Segment(const std::string &iStartingPoint)
    {
      std::vector< std::string > wSplit;
      boost::split(wSplit, iStartingPoint, boost::is_any_of(","));
      mStartingIndex = static_cast< uint16_t >(std::strtoul(wSplit[0].c_str(), nullptr, 10));
      mStartingValue = std::strtod(wSplit[1].c_str(), nullptr);
    }
    void setEndPoint(const std::string &iStartingPoint)
    {
      std::vector< std::string > wSplit;
      boost::split(wSplit, iStartingPoint, boost::is_any_of(","));
      mEndingIndex = static_cast< uint16_t >(std::strtoul(wSplit[0].c_str(), nullptr, 10));
      mEndingValue = std::strtod(wSplit[1].c_str(), nullptr);
      mSlope = (mEndingValue - mStartingValue) / static_cast< double >(mEndingIndex - mStartingIndex);
      mInitialValue = mEndingValue - mSlope * static_cast< double >(mEndingIndex);
    }
    int16_t mStartingIndex;
    int16_t mEndingIndex;
    double mStartingValue;
    double mEndingValue;
    double mSlope;
    double mInitialValue;
  };
  std::vector< Segment > wSegments;
  std::ifstream wCalibration(iCalibrationFile);
  if (!wCalibration)
  {
    throw std::runtime_error(std::string("Calibration file not found: ").append(iCalibrationFile));
  }
  std::string wLine;
  std::getline(wCalibration, wLine);
  wSegments.emplace_back(wLine);
  while (std::getline(wCalibration, wLine))
  {
    wSegments.back().setEndPoint(wLine);
    wSegments.emplace_back(wLine);
  }
  wSegments.pop_back();
  auto wActualSegment = wSegments.begin();
  auto wLastSegment = wSegments.end() - 1;
  for (int32_t wIndex = std::numeric_limits< int16_t >::min(); wIndex <= std::numeric_limits< int16_t >::max(); ++wIndex)
  {
    if (wIndex > wActualSegment->mEndingIndex && wActualSegment != wLastSegment)
    {
      ++wActualSegment;
    }
    mTemperatureCalibration[static_cast< uint16_t >(wIndex)] = wActualSegment->mSlope * static_cast< double >(wIndex) + wActualSegment->mInitialValue;
  }
}

uint16_t TemperatureSampling::sampleTemperatureIndex(const Mcp3424 &iMcp3424) const
{
  return iMcp3424.read();
}

double TemperatureSampling::sample() const
{
  return mTemperatureCalibration[sampleTemperatureIndex(mMcp3424)];
}
