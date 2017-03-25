#pragma once

#include "TcpServer/ISender.h"
#include "KegeratorMetrics.h"
#include "Subject.h"

#include <boost/algorithm/string.hpp>

class ConnectionHandler : public IObserver< KegeratorMetrics >
{
public:
  ConnectionHandler(DataActiveObject< KegeratorMetrics > &iKegeratorMetrics)
    : mKegeratorMetrics(iKegeratorMetrics)
  {
    mKegeratorMetrics.getConstInternal().attach(this);
  }

  ConnectionHandler(const ConnectionHandler &iConnectionHandler)
    : mKegeratorMetrics(iConnectionHandler.mKegeratorMetrics)
  {
    mKegeratorMetrics.getConstInternal().attach(this);
  }

  ~ConnectionHandler()
  {
    mKegeratorMetrics.getConstInternal().detach(this);
  }

  void setSender(const std::shared_ptr< ISender > &iSender)
  {
    mSender = iSender;
  }

  void operator()(const std::string &iPayload)
  {
    handleMessage(iPayload);
  }

  void update(const KegeratorMetrics &iMetrics)
  {
    auto wSender = mSender.lock();
    if (wSender != nullptr)
    {
      wSender->send(iMetrics.dataString());
    }
  }

private:
  void handleMessage(const std::string &iPayload)
  {
    if (iPayload == "get_status")
    {
      mKegeratorMetrics.pushDataAccess([&](KegeratorMetrics &iMetrics)
      {
        update(iMetrics);
      });
    }
    else
    {
      std::vector< std::string > wSplit;
      boost::split(wSplit, iPayload, boost::is_any_of("|"));
      if (wSplit.size() == 3 && wSplit[0] == "set_keg_level")
      {
        auto wKegIndex = std::strtoul(wSplit[1].c_str(), nullptr, 10);
        auto wLevel = std::strtod(wSplit[2].c_str(), nullptr) / 100.0;
        mKegeratorMetrics.pushDataAccess([=](KegeratorMetrics &iMetrics)
        {
          iMetrics.setKegLevel(wKegIndex, wLevel);
        });
      }
    }
  }

  std::weak_ptr< ISender > mSender;
  DataActiveObject< KegeratorMetrics > &mKegeratorMetrics;
};
