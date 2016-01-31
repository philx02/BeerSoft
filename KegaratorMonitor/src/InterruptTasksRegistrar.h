#pragma once

#include <vector>
#include <functional>

template< typename Signature >
class InterruptTasksRegistrar
{
public:
  typedef std::vector< std::function< Signature > > InterruptTasks;

  static InterruptTasksRegistrar & getInstance();

  inline void addInterruptTask(std::function< Signature > &&iFunction)
  {
    mInterruptTasks.emplace_back(std::move(iFunction));
  }

  inline typename std::vector< std::function< Signature > >::const_iterator begin()
  {
    return std::cbegin(mInterruptTasks);
  }
  
  inline typename std::vector< std::function< Signature > >::const_iterator end()
  {
    return std::cend(mInterruptTasks);
  }

  inline typename InterruptTasks::size_type size()
  {
    return mInterruptTasks.size();
  }

private:
  InterruptTasks mInterruptTasks;
};
