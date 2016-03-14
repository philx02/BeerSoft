#pragma once

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <fstream>

class HeaterControl
{
public:
  HeaterControl(const char *iPwmGpio)
    : mPwmGpio(iPwmGpio)
    , mTemperatureCommand(0)
    , mActualTemperature(0)
    , mIoService(std::make_unique< boost::asio::io_service >())
    , mDeadlineTimer(makeDeadlineTimer())
    , mPwmThread(std::make_unique< std::thread >([&]() { mIoService->run(); }))
  {
  }

  HeaterControl(HeaterControl &&iHeaterControl)
    : mPwmGpio(std::move(iHeaterControl.mPwmGpio))
    , mTemperatureCommand(iHeaterControl.mTemperatureCommand)
    , mActualTemperature(iHeaterControl.mActualTemperature)
    , mIoService(std::move(iHeaterControl.mIoService))
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
  std::unique_ptr< boost::asio::deadline_timer > makeDeadlineTimer()
  {
    auto wDeadlineTimer = std::make_unique< boost::asio::deadline_timer >(*mIoService, boost::posix_time::milliseconds(10));
    wDeadlineTimer->async_wait([](const boost::system::error_code &) {});
    return wDeadlineTimer;
  }

  std::ofstream mPwmGpio;
  double mTemperatureCommand;
  double mActualTemperature;
  std::unique_ptr< boost::asio::io_service > mIoService;
  std::unique_ptr< boost::asio::deadline_timer > mDeadlineTimer;
  std::unique_ptr< std::thread > mPwmThread;
};
