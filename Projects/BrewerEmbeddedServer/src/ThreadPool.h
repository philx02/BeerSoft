#pragma once

#include <boost/asio.hpp>

#include <vector>
#include <thread>

namespace ba = boost::asio;

class ThreadPool
{
public:
  ThreadPool(std::size_t iNumberOfThreads)
    : mWorking(new ba::io_service::work(mIoService))
  {
    for (std::size_t wIter = 0; wIter < iNumberOfThreads; ++wIter)
    {
      mWorkers.emplace_back([&] { mIoService.run(); });
    }
  }

  ~ThreadPool()
  {
    stop();
  }

  void stop()
  {
    mIoService.stop();
    for (auto &wWorker : mWorkers)
    {
      if (wWorker.joinable())
      {
        wWorker.join();
      }
    }
  }

  template< typename F >
  void enqueue(F &&iWork) const
  {
    mIoService.post(iWork);
  }

private:
  std::vector< std::thread > mWorkers;

  mutable ba::io_service mIoService;
  std::unique_ptr< ba::io_service::work > mWorking;
};
