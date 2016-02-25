#pragma once

#include <vector>
#include <functional>

template< typename Signature >
class SamplingTasksRegistrar
{
public:
  typedef std::vector< std::function< Signature > > SamplingTasks;

  static SamplingTasksRegistrar & getInstance();

  inline void addSamplingTask(std::function< Signature > &&iFunction)
  {
    mSamplingTasks.emplace_back(std::move(iFunction));
  }

  inline typename std::vector< std::function< Signature > >::const_iterator begin()
  {
    return std::begin(mSamplingTasks);
  }
  
  inline typename std::vector< std::function< Signature > >::const_iterator end()
  {
    return std::end(mSamplingTasks);
  }

  inline typename SamplingTasks::size_type size()
  {
    return mSamplingTasks.size();
  }

private:
  SamplingTasks mSamplingTasks;
};
