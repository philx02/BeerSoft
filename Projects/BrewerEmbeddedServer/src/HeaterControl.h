#pragma once

#include "PeriodicTimer.h"

#include <fstream>
#include <atomic>
#include <chrono>

class HeaterControl : public boost::noncopyable
{
public:
  HeaterControl(const char *iPwmGpio, const std::chrono::milliseconds &iMinimumIncrement, const std::chrono::milliseconds &iPwmPeriod, size_t iMinimalPwmPct, size_t iMaximalPwmPct, double iKp, double iKi = 0.0, double iKd = 0.0)
    : mPwmGpio(std::make_unique< std::ofstream >(iPwmGpio))
    , mTemperatureCommand(0)
    , mActualTemperature(0)
    , mDutyCycle(std::chrono::milliseconds::zero())
    , mMinimumIncrement(iMinimumIncrement)
    , mPwmPeriod(iPwmPeriod)
    , mPwmPeriodProgress(std::chrono::milliseconds::zero())
    , mMinimalDutyCycle(iPwmPeriod * iMinimalPwmPct / 100)
    , mMaximalDutyCycle(iPwmPeriod * iMaximalPwmPct / 100)
    , mIoService(std::make_unique< boost::asio::io_service >())
    , mHandleIteration([&]() -> bool { handleIteration(); return true; })
    , mPeriodicTimer(createPeriodicTimer(*mIoService, mHandleIteration, iMinimumIncrement))
    , mControlCommandSource(TEMPERATURE)
    , mKp(iKp)
    , mMode(OFF)
    , mGpioValue(GPIO_OFF)
  {
    setGpio(GPIO_OFF, true);
    mPeriodicTimer->start();
    mPwmThread = std::make_unique< std::thread >([&]() { mIoService->run(); });
  }

  void setTemperatureCommand(double iTemperature)
  {
    if (iTemperature > 110)
    {
      mTemperatureCommand = 110;
    }
    else if (iTemperature < 0)
    {
      mTemperatureCommand = 0;
    }
    else
    {
      mTemperatureCommand = iTemperature;
    }
    mControlCommandSource = TEMPERATURE;
  }

  void setDutyCycleCommand(double iDutyCycle)
  {
    mDutyCycleCommand = convertDutyCycle(static_cast< size_t >(iDutyCycle));
    mControlCommandSource = DUTY_CYCLE;
  }

  void setActualTemperature(double iTemperature)
  {
    mActualTemperature = iTemperature;
  }

  double getTemperatureCommand() const
  {
    return mTemperatureCommand;
  }

  double getActualTemperature() const
  {
    return mActualTemperature;
  }

  double getActualDutyCycle() const
  {
    return static_cast< double >(mDutyCycle.load().count()) / static_cast< double >(mPwmPeriod.count());
  }

  enum eMode
  {
    OFF,
    PWM,
    ALWAYS_ON
  };

  void setMode(eMode iMode)
  {
    mMode = iMode;
  }

  eMode getMode() const
  {
    return mMode;
  }

  enum eControlCommandSource
  {
    TEMPERATURE,
    DUTY_CYCLE
  };

  eControlCommandSource getControlCommandSource() const
  {
    return mControlCommandSource;
  }

private:
  std::chrono::milliseconds convertDutyCycle(size_t iPercentage)
  {
    auto wDutyCycle = (mPwmPeriod * iPercentage) / 100;
    if (wDutyCycle > mMaximalDutyCycle)
    {
      wDutyCycle = mPwmPeriod;
    }
    else if (wDutyCycle > std::chrono::milliseconds::zero() && wDutyCycle < mMinimalDutyCycle)
    {
      wDutyCycle = mMinimalDutyCycle;
    }
    return wDutyCycle;
  }

  std::chrono::milliseconds computeDutyCycleInPwmMode()
  {
    switch (mControlCommandSource)
    {
    case TEMPERATURE:
      {
        auto wTemperatureError = mTemperatureCommand.load() - mActualTemperature.load();
        if (wTemperatureError < 0)
        {
          wTemperatureError = 0;
        }
        return convertDutyCycle(static_cast< size_t >(mKp * wTemperatureError));
      }
    case DUTY_CYCLE:
    default:
      return mDutyCycleCommand.load();
      break;
    }
  }

  void computeDutyCycle()
  {
    switch (mMode)
    {
    case PWM:
      mDutyCycle = computeDutyCycleInPwmMode();
      break;
    case ALWAYS_ON:
      mDutyCycle = mPwmPeriod;
      break;
    case OFF:
    default:
      mDutyCycle = std::chrono::milliseconds::zero();
      break;
    }
  }

  void handleIteration()
  {
    mPwmPeriodProgress += mMinimumIncrement;
    if (mPwmPeriodProgress >= mPwmPeriod)
    {
      computeDutyCycle();
      mPwmPeriodProgress = std::chrono::milliseconds::zero();
      if (mDutyCycle.load() != std::chrono::milliseconds::zero())
      {
        setGpio(GPIO_ON);
      }
    }
    else if (mPwmPeriodProgress.count() >= mDutyCycle.load().count())
    {
      setGpio(GPIO_OFF);
    }
  }

  void setGpio(char iGpioValue, bool iForce = false)
  {
    if (mGpioValue != iGpioValue || iForce)
    {
      mGpioValue = iGpioValue;
      *mPwmGpio << mGpioValue;
      mPwmGpio->flush();
    }
  }

  std::unique_ptr< std::ofstream > mPwmGpio;
  std::atomic< double > mTemperatureCommand;
  std::atomic< double > mActualTemperature;
  std::atomic< std::chrono::milliseconds > mDutyCycle;
  std::atomic< std::chrono::milliseconds > mDutyCycleCommand;
  std::chrono::milliseconds mMinimumIncrement;
  std::chrono::milliseconds mPwmPeriod;
  std::chrono::milliseconds mPwmPeriodProgress;
  std::chrono::milliseconds mMinimalDutyCycle;
  std::chrono::milliseconds mMaximalDutyCycle;
  std::unique_ptr< boost::asio::io_service > mIoService;
  std::function< bool (void) > mHandleIteration;
  std::shared_ptr< PeriodicTimer< decltype(mHandleIteration) > > mPeriodicTimer;
  std::unique_ptr< std::thread > mPwmThread;
  std::atomic< eControlCommandSource > mControlCommandSource;

  double mKp;
  double mKi;
  double mKd;
  std::atomic< eMode > mMode;
  char mGpioValue;

  static const char GPIO_OFF = '0';
  static const char GPIO_ON = '1';
};
