#pragma once

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <fstream>
#include <atomic>

class HeaterControl
{
public:
  HeaterControl(const char *iPwmGpio, const boost::posix_time::time_duration &iMinimumIncrement, const boost::posix_time::time_duration &iPwmPeriod, size_t iMinimalPwmPct)
    : mPwmGpio(iPwmGpio)
    , mTemperatureCommand(0)
    , mActualTemperature(0)
    , mMinimumIncrement(iMinimumIncrement)
    , mPwmPeriod(iPwmPeriod)
    , mMinimalDutyCycle(iPwmPeriod * iMinimalPwmPct / 100)
    , mIoService(std::make_unique< boost::asio::io_service >())
    , mDeadlineTimer(makeDeadlineTimer(iMinimumIncrement))
    , mPwmThread(std::make_unique< std::thread >([&]() { mIoService->run(); }))
  {
  }

  HeaterControl(HeaterControl &&iHeaterControl)
    : mPwmGpio(std::move(iHeaterControl.mPwmGpio))
    , mTemperatureCommand(iHeaterControl.mTemperatureCommand.load())
    , mActualTemperature(iHeaterControl.mActualTemperature.load())
    , mIoService(std::move(iHeaterControl.mIoService))
    , mDutyCycle(std::move(iHeaterControl.mDutyCycle))
    , mMinimumIncrement(std::move(iHeaterControl.mMinimumIncrement))
    , mPwmPeriod(std::move(iHeaterControl.mPwmPeriod))
    , mPwmPeriodProgress(std::move(iHeaterControl.mPwmPeriodProgress))
    , mMinimalDutyCycle(std::move(iHeaterControl.mMinimalDutyCycle))
    , mDeadlineTimer(std::move(iHeaterControl.mDeadlineTimer))
    , mPwmThread(std::move(iHeaterControl.mPwmThread))
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

private:
  std::unique_ptr< boost::asio::deadline_timer > makeDeadlineTimer(const boost::posix_time::time_duration &iMinimumIncrement)
  {
    auto wDeadlineTimer = std::make_unique< boost::asio::deadline_timer >(*mIoService, iMinimumIncrement);
    wDeadlineTimer->async_wait([&](const boost::system::error_code &iError) { handleIteration(iError); });
    return wDeadlineTimer;
  }

  void computeDutyCycle()
  {
    auto wTemperatureError = mTemperatureCommand.load() - mActualTemperature.load();
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
      mPwmGpio << (mDutyCycle > mMinimalDutyCycle) ? '1' : '0';
    }
    else if (mPwmPeriodProgress >= mDutyCycle)
    {
      mPwmGpio << '0';
    }
    mDeadlineTimer->async_wait([&](const boost::system::error_code &iError) { handleIteration(iError); });
  }

  std::ofstream mPwmGpio;
  std::atomic< double > mTemperatureCommand;
  std::atomic< double > mActualTemperature;
  boost::posix_time::time_duration mDutyCycle;
  boost::posix_time::time_duration mMinimumIncrement;
  boost::posix_time::time_duration mPwmPeriod;
  boost::posix_time::time_duration mPwmPeriodProgress;
  boost::posix_time::time_duration mMinimalDutyCycle;
  std::unique_ptr< boost::asio::io_service > mIoService;
  std::unique_ptr< boost::asio::deadline_timer > mDeadlineTimer;
  std::unique_ptr< std::thread > mPwmThread;
};
