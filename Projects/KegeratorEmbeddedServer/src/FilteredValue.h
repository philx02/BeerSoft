#pragma once

#include <atomic>

class FilteredValue
{
public:
  FilteredValue(double iInitialValue, double iFactor)
    : mActualValue(iInitialValue)
    , mFactor(iFactor)
  {
  }

  FilteredValue(const FilteredValue &iFilteredValue)
    : mActualValue(iFilteredValue.getActualValue())
    , mFactor(iFilteredValue.mFactor)
  {
  }

  void addSample(double iValue)
  {
    mActualValue.store((iValue - mActualValue.load()) * mFactor);
  }

  double getActualValue() const
  {
    return mActualValue.load();
  }

private:
  std::atomic< double > mActualValue;
  double mFactor;
};
