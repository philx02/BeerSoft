#pragma once

#include "IObserver.h"

#include <vector>
#include <algorithm>
#include <mutex>

template< typename SubjectType >
class Subject
{
public:
  Subject()
  {
  }

  Subject(Subject &&iSubject)
    : mMutex(std::move(iSubject.mMutex))
    , mObservers(std::move(iSubject.mObservers))
  {
  }

  inline void attach(IObserver< SubjectType > *iObserver) const
  {
    std::lock_guard< std::mutex > wLock(mMutex);
    mObservers.emplace_back(iObserver);
  }
  inline void detach(IObserver< SubjectType > *iObserver) const
  {
    std::lock_guard< std::mutex > wLock(mMutex);
    mObservers.erase(std::remove(mObservers.begin(), mObservers.end(), iObserver), mObservers.end());
  }

protected:
  inline void notify(const SubjectType &iThis) const
  {
    std::lock_guard< std::mutex > wLock(mMutex);
    for (auto && iObserver : mObservers)
    {
      iObserver->update(iThis);
    }
  }

private:
  mutable std::mutex mMutex;
  mutable std::vector< IObserver< SubjectType > * > mObservers;
};
