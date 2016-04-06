#pragma once

#include "PeriodicTimer.h"
#include "Subject.h"

#include <fstream>
#include <atomic>
#include <chrono>
#include <memory>

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
    , mTimerCount(std::chrono::seconds(0))
    , mTimerLimit(std::chrono::seconds(0))
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
    mMode = iMode;
    switch (mMode)
    {
    case OFF:
      setGpio(GPIO_OFF);
      break;
    case PUMP_UNTIL:
      if (mTimerCount.load() >= mTimerLimit.load())
      {
        setMode(OFF);
        return;
      }
      else if (mTimerCount.load().count() > 0) // resuming
      {
        setGpio(GPIO_ON);
        return;
      }
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
    return mTimerLimit.load() - mTimerCount.load();
  }

  void setTimerLimit(const std::chrono::seconds &iDuration)
  {
    mTimerLimit = iDuration;
    mTimerCount = std::chrono::seconds(0);
  }

private:
  bool handleIteration()
  {
    bool wReturnValue = true;
    if (mMode.load() != PUMP_UNTIL)
    {
      return wReturnValue;
    }
    mTimerCount.store(mTimerCount.load() + std::chrono::seconds(1));
    if (mTimerCount.load() >= mTimerLimit.load())
    {
      setMode(OFF);
      wReturnValue = false;
    }
    Subject< PumpControl >::notify(*this);
    return wReturnValue;
  }

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
  std::unique_ptr< boost::asio::io_service > mIoService;
  std::function< bool (void) > mHandleIteration;
  std::shared_ptr< PeriodicTimer< decltype(mHandleIteration) > > mPeriodicTimer;
  std::unique_ptr< std::thread > mTimerThread;
  std::atomic< std::chrono::seconds > mTimerCount;
  std::atomic< std::chrono::seconds > mTimerLimit;

  static const char GPIO_OFF = '0';
  static const char GPIO_ON = '1';
};
