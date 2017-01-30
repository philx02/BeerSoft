#pragma once

#include <mutex>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>

namespace bac = boost::accumulators;

class MovingAveragedValue
{
public:
  MovingAveragedValue(size_t iWindowSize)
    : mAcc(bac::tag::rolling_window::window_size = iWindowSize)
  {
  }

  MovingAveragedValue(const MovingAveragedValue &iValue)
    : mAcc(iValue.mAcc)
  {
  }

  void addSample(double iValue)
  {
    std::lock_guard< decltype(mAccMutex) > wLock(mAccMutex);
    mAcc(iValue);
  }

  double getActualValue() const
  {
    std::lock_guard< decltype(mAccMutex) > wLock(mAccMutex);
    return bac::rolling_mean(mAcc);
  }

private:
  mutable std::mutex mAccMutex;
  bac::accumulator_set< double, bac::stats< bac::tag::rolling_mean > > mAcc;
};
