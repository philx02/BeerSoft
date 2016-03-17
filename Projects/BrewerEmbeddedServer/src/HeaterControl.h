#pragma once

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <fstream>
#include <atomic>

class HeaterControl
{
public:
  HeaterControl(const char *iPwmGpio, const boost::posix_time::time_duration &iMinimumIncrement, const boost::posix_time::time_duration &iPwmPeriod, size_t iMinimalPwmPct, double iKp)
    : mPwmGpio(iPwmGpio)
    , mTemperatureCommand(0)
    , mActualTemperature(0)
    , mMinimumIncrement(iMinimumIncrement)
    , mPwmPeriod(iPwmPeriod)
    , mMinimalDutyCycle(iPwmPeriod * iMinimalPwmPct / 100)
    , mIoService(std::make_unique< boost::asio::io_service >())
    , mDeadlineTimer(makeDeadlineTimer(iMinimumIncrement))
    , mPwmThread(std::make_unique< std::thread >([&]() { mIoService->run(); }))
    , mKp(iKp)
    , mMode(OFF)
    , mGpioValue(GPIO_OFF)
  {
  }

  HeaterControl(HeaterControl &&iHeaterControl)
    : mPwmGpio(std::move(iHeaterControl.mPwmGpio))
    , mTemperatureCommand(iHeaterControl.mTemperatureCommand.load())
    , mActualTemperature(iHeaterControl.mActualTemperature.load())
    , mIoService(std::move(iHeaterControl.mIoService))
    , mDutyCycle(iHeaterControl.mDutyCycle.load())
    , mMinimumIncrement(std::move(iHeaterControl.mMinimumIncrement))
    , mPwmPeriod(std::move(iHeaterControl.mPwmPeriod))
    , mPwmPeriodProgress(std::move(iHeaterControl.mPwmPeriodProgress))
    , mMinimalDutyCycle(std::move(iHeaterControl.mMinimalDutyCycle))
    , mDeadlineTimer(std::move(iHeaterControl.mDeadlineTimer))
    , mPwmThread(std::move(iHeaterControl.mPwmThread))
    , mKp(iHeaterControl.mKp)
    , mMode(iHeaterControl.mMode.load())
    , mGpioValue(iHeaterControl.mGpioValue)
  {
  }

  void setTemperatureCommand(double iTemperature)
  {
    mTemperatureCommand = iTemperature;
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
    return static_cast< double >(mDutyCycle.load().total_microseconds()) / static_cast< double >(mPwmPeriod.total_microseconds());
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

private:
  std::unique_ptr< boost::asio::deadline_timer > makeDeadlineTimer(const boost::posix_time::time_duration &iMinimumIncrement)
  {
    auto wDeadlineTimer = std::make_unique< boost::asio::deadline_timer >(*mIoService, iMinimumIncrement);
    wDeadlineTimer->async_wait([&](const boost::system::error_code &iError) { handleIteration(iError); });
    return wDeadlineTimer;
  }

  void computeDutyCycle()
  {
    switch (mMode)
    {
    case PWM:
      {
        auto wTemperatureError = mTemperatureCommand.load() - mActualTemperature.load();
        if (wTemperatureError < 0)
        {
          wTemperatureError = 0;
        }
        mDutyCycle = mPwmPeriod * static_cast< size_t >(mKp * wTemperatureError) / 100;
        if (mDutyCycle > mPwmPeriod)
        {
          mDutyCycle = mPwmPeriod;
        }
      }
      break;
    case ALWAYS_ON:
      mDutyCycle = mPwmPeriod;
      break;
    case OFF:
    default:
      mDutyCycle = boost::posix_time::time_duration();
      break;
    }
  }

  void handleIteration(const boost::system::error_code &iError)
  {
    if (iError)
    {
      return;
    }
    mPwmPeriodProgress += mMinimumIncrement;
    if (mPwmPeriodProgress >= mPwmPeriod)
    {
      computeDutyCycle();
      mPwmPeriodProgress = boost::posix_time::time_duration();
      setGpio((mDutyCycle > mMinimalDutyCycle) ? GPIO_ON : GPIO_OFF);
    }
    else if (mPwmPeriodProgress >= mDutyCycle)
    {
      setGpio(GPIO_OFF);
    }
    mDeadlineTimer->async_wait([&](const boost::system::error_code &iError) { handleIteration(iError); });
  }

  void setGpio(char iGpioValue)
  {
    if (mGpioValue != iGpioValue)
    {
      mGpioValue = iGpioValue;
      mPwmGpio << mGpioValue;
    }
  }

  std::ofstream mPwmGpio;
  std::atomic< double > mTemperatureCommand;
  std::atomic< double > mActualTemperature;
  std::atomic< boost::posix_time::time_duration > mDutyCycle;
  boost::posix_time::time_duration mMinimumIncrement;
  boost::posix_time::time_duration mPwmPeriod;
  boost::posix_time::time_duration mPwmPeriodProgress;
  boost::posix_time::time_duration mMinimalDutyCycle;
  std::unique_ptr< boost::asio::io_service > mIoService;
  std::unique_ptr< boost::asio::deadline_timer > mDeadlineTimer;
  std::unique_ptr< std::thread > mPwmThread;

  double mKp;
  std::atomic< eMode > mMode;
  char mGpioValue;

  static const char GPIO_OFF = '0';
  static const char GPIO_ON = '1';
};
