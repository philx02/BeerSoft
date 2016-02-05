#pragma once

#include <boost/noncopyable.hpp>

#include <functional>
#include <deque>
#include <mutex>
#include <thread>
#include <condition_variable>

class ActiveObject : boost::noncopyable
{
public:
  typedef std::function< void () > Function;

  ~ActiveObject()
  {
    stop();
  }

  inline void run()
  {
    workHandler();
  }

  inline void stop()
  {
    doPush(true, []() {});
  }

  inline void push(const Function &iFunction)
  {
    doPush(false, iFunction);
  }

private:
  typedef std::deque< std::pair< bool, Function > > WorkQueue;
  mutable WorkQueue mWorkQueue;
  mutable std::mutex mQueueMutex;
  mutable std::condition_variable mWorkSignal;

  void doPush(bool iTerminate, const Function &iFunction) const
  {
    {
      std::lock_guard< std::mutex > wLock(mQueueMutex);
      mWorkQueue.emplace_back(iTerminate, iFunction);
    }
    mWorkSignal.notify_one();
  }

  void workHandler()
  {
    while (true)
    {
      std::unique_lock< std::mutex > wLock(mQueueMutex);
      mWorkSignal.wait(wLock, [&]() { return !mWorkQueue.empty(); });
      auto &&wWorkItem = mWorkQueue.front();
      if (wWorkItem.first)
      {
        break;
      }
      wWorkItem.second();
      mWorkQueue.pop_front();
    }
  }
};

template< typename T >
class DataActiveObject : public ActiveObject
{
public:
  DataActiveObject(T &&iInternal)
    : mInternal(std::move(iInternal))
  {
  }

  DataActiveObject(DataActiveObject< T > &&iDataActiveObject)
    : mInternal(std::move(iDataActiveObject.mInternal))
  {
  }

  inline void dataPush(const std::function< void (T &) > &iFunction)
  {
    push([=]() { iFunction(mInternal); });
  }

  const T & getConstInternal() const
  {
    return mInternal;
  }

private:
  T mInternal;
};