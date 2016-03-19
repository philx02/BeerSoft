#pragma once

#include <boost/asio.hpp>

#include <memory>
#include <chrono>
#include <iostream>
#include <atomic>

namespace ba = boost::asio;

template< typename Callback >
class PeriodicTimer;

template< typename Callback >
std::shared_ptr< PeriodicTimer< Callback > > createPeriodicTimer(ba::io_service &, const Callback &, const std::chrono::milliseconds &);

template< typename Callback >
class PeriodicTimer : public std::enable_shared_from_this< PeriodicTimer< Callback > >
{
public:
  ~PeriodicTimer()
  {
    stop();
  }

  void start()
  {
    if (!mRunning.exchange(true))
    {
      mTimer.expires_from_now(boost::posix_time::millisec(mPeriod.count()));
      auto wThis = std::enable_shared_from_this< PeriodicTimer< Callback > >::shared_from_this();
      mTimer.async_wait([=](const boost::system::error_code& iError) {wThis->handler(iError);}); 
    }
  }

  void stop()
  {
    mRunning.store(false);
    mTimer.cancel();
  }

private:
  friend std::shared_ptr< PeriodicTimer< Callback > > createPeriodicTimer< Callback >(ba::io_service &, const Callback &, const std::chrono::milliseconds &);
  
  PeriodicTimer(ba::io_service& iIoService, Callback iCallBack, std::chrono::milliseconds iPeriod) 
    : mTimer(iIoService)
    , mCallBack(std::move(iCallBack))
    , mPeriod(std::move(iPeriod))
  {
    mRunning.store(false);
  }

  void doStart() 
  { 
    mTimer.expires_at(mTimer.expires_at() + boost::posix_time::millisec(mPeriod.count()));
    auto wThis = std::enable_shared_from_this< PeriodicTimer< Callback > >::shared_from_this();
    mTimer.async_wait([=](const boost::system::error_code& iError) {wThis->handler(iError);}); 
  }

  void handler(const boost::system::error_code& iError)
  {
    if (!mRunning)
    {
      return;
    }

    if (iError) 
    {
      std::cerr << iError.message() << std::endl;
      return;
    }
    
    mCallBack();
    doStart();
  }

  ba::deadline_timer mTimer;
  Callback mCallBack;
  std::atomic< bool > mRunning;
  std::chrono::milliseconds mPeriod;
};

template< typename Callback >
std::shared_ptr< PeriodicTimer< Callback > > createPeriodicTimer(ba::io_service &iIoService, const Callback &iCallBack, const std::chrono::milliseconds &iPeriod)
{
  // cannot use make_shared because it is not friend
  return std::shared_ptr< PeriodicTimer< Callback > >(new PeriodicTimer< Callback >(iIoService, iCallBack, iPeriod));
}

