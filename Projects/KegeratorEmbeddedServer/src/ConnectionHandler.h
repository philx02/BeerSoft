#pragma once

#include "TcpServer/ISender.h"
#include "KegeratorMetrics.h"
#include "Subject.h"

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
        update(mKegeratorMetrics.getConstInternal());
      });
    }
  }

  std::weak_ptr< ISender > mSender;
  DataActiveObject< KegeratorMetrics > &mKegeratorMetrics;
};
