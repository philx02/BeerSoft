#pragma once

#include "PeriodicTimer.h"

#include <fstream>
#include <atomic>
#include <chrono>

class PumpControl
{
public:
  PumpControl(const char *iDopGpio)
    : mDopGpio(std::make_unique< std::ofstream >(iDopGpio))
    , mMode(OFF)
    , mGpioValue(GPIO_OFF)
  {
    setGpio(GPIO_OFF, true);
  }

  PumpControl(PumpControl &&iPumpControl)
    : mDopGpio(std::move(iPumpControl.mDopGpio))
    , mMode(iPumpControl.mMode.load())
    , mGpioValue(iPumpControl.mGpioValue)
  {
  }

  enum eMode
  {
    OFF,
    PUMP_UNTIL,
    ALWAYS_ON
  };

  void setMode(eMode iMode)
  {
    mMode = iMode;
    switch (mMode)
    {
    case OFF:
      setGpio(GPIO_OFF);
      break;
    case PUMP_UNTIL:
      break;
    case ALWAYS_ON:
      setGpio(GPIO_ON);
      break;
    default:
      break;
    }
  }

  eMode getMode() const
  {
    return mMode;
  }

private:
  //void handleIteration()
  //{
  //  mPwmPeriodProgress += mMinimumIncrement;
  //  if (mPwmPeriodProgress >= mPwmPeriod)
  //  {
  //    mPwmPeriodProgress = std::chrono::milliseconds::zero();
  //    if (mDutyCycle.load() != std::chrono::milliseconds::zero())
  //    {
  //      setGpio(GPIO_ON);
  //    }
  //  }
  //  else if (mPwmPeriodProgress.count() >= mDutyCycle.load().count())
  //  {
  //    setGpio(GPIO_OFF);
  //  }
  //}

  void setGpio(char iGpioValue, bool iForce = false)
  {
    if (mGpioValue != iGpioValue || iForce)
    {
      mGpioValue = iGpioValue;
      *mDopGpio << mGpioValue;
      mDopGpio->flush();
    }
  }

  std::unique_ptr< std::ofstream > mDopGpio;
  std::atomic< eMode > mMode;
  char mGpioValue;

  static const char GPIO_OFF = '0';
  static const char GPIO_ON = '1';
};
