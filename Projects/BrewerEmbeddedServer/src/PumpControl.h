#pragma once

#include "PeriodicTimer.h"
#include "Subject.h"

#include <fstream>
#include <chrono>
#include <memory>
#include <mutex>

class PumpControl : public Subject< PumpControl >, public boost::noncopyable
{
public:
  PumpControl(const char *iDopGpio)
    : mDopGpio(std::make_unique< std::ofstream >(iDopGpio))
    , mMode(OFF)
    , mGpioValue(GPIO_OFF)
    , mIoService(std::make_unique< boost::asio::io_service >())
    , mHandleIteration([&]() -> bool { return handleIteration(); })
    , mPeriodicTimer(createPeriodicTimer(*mIoService, mHandleIteration, std::chrono::seconds(1)))
    , mCounting(false)
    , mTimerCount(std::chrono::seconds::zero())
    , mTimerLimit(std::chrono::seconds::zero())
  {
    setGpio(GPIO_OFF, true);
  }

  enum eMode
  {
    OFF,
    PUMP_UNTIL,
    ALWAYS_ON
  };

  void setMode(eMode iMode)
  {
    Lock wLock(mMutex);
    mMode = iMode;
    switch (mMode)
    {
    case OFF:
      setGpio(GPIO_OFF);
      break;
    case PUMP_UNTIL:
      if (mTimerCount >= mTimerLimit)
      {
        setMode(OFF);
        return;
      }
      else if (mCounting) // resuming
      {
        setGpio(GPIO_ON);
        return;
      }
      mCounting = true;
      setGpio(GPIO_ON);
      if (mTimerThread != nullptr)
      {
        mTimerThread->join();
      }
      mPeriodicTimer->stop();
      mIoService->reset();
      mPeriodicTimer->start();
      mTimerThread = std::make_unique< std::thread >([&]() { mIoService->run(); });
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

  std::chrono::seconds getRemainingTime() const
  {
    Lock wLock(mMutex);
    return mTimerLimit - mTimerCount;
  }

  void setTimerLimit(const std::chrono::seconds &iDuration)
  {
    Lock wLock(mMutex);
    mTimerLimit = iDuration;
    mTimerCount = std::chrono::seconds::zero();
  }

private:
  bool handleIteration()
  {
    Lock wLock(mMutex);
    bool wReturnValue = true;
    if (mMode != PUMP_UNTIL)
    {
      return wReturnValue;
    }
    mTimerCount = mTimerCount + std::chrono::seconds(1);
    if (mTimerCount >= mTimerLimit)
    {
      mCounting = false;
      setMode(OFF);
      wReturnValue = false;
    }
    Subject< PumpControl >::notify(*this);
    return wReturnValue;
  }

  void setGpio(char iGpioValue, bool iForce = false)
  {
    Lock wLock(mMutex);
    if (mGpioValue != iGpioValue || iForce)
    {
      mGpioValue = iGpioValue;
      *mDopGpio << mGpioValue;
      mDopGpio->flush();
    }
  }

  std::unique_ptr< std::ofstream > mDopGpio;
  eMode mMode;
  char mGpioValue;
  std::unique_ptr< boost::asio::io_service > mIoService;
  std::function< bool (void) > mHandleIteration;
  std::shared_ptr< PeriodicTimer< decltype(mHandleIteration) > > mPeriodicTimer;
  std::unique_ptr< std::thread > mTimerThread;
  bool mCounting;
  std::chrono::seconds mTimerCount;
  std::chrono::seconds mTimerLimit;
  mutable std::recursive_mutex mMutex;
  typedef std::lock_guard< decltype(mMutex) > Lock;

  static const char GPIO_OFF = '0';
  static const char GPIO_ON = '1';
};
