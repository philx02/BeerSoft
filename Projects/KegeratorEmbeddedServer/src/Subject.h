#pragma once

#include "IObserver.h"

#include <vector>
#include <algorithm>

template< typename SubjectType >
class Subject
{
public:
  inline void attach(IObserver< SubjectType > *iObserver) const
  {
    mObservers.emplace_back(iObserver);
  }
  inline void detach(IObserver< SubjectType > *iObserver) const
  {
    mObservers.erase(std::remove(mObservers.begin(), mObservers.end(), iObserver), mObservers.end());
  }

protected:
  inline void notify(const SubjectType &iThis) const
  {
    for (auto && iObserver : mObservers)
    {
      iObserver->update(iThis);
    }
  }

private:
  mutable std::vector< IObserver< SubjectType > * > mObservers;
};
