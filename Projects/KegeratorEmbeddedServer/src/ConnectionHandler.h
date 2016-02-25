#pragma once

#include "TcpServer/ISender.h"
#include "KegeratorMetrics.h"

class ConnectionHandler : public IObserver< KegeratorMetrics >
{
public:
  ConnectionHandler(const KegeratorMetrics &iKegeratorMetrics)
    : mKegeratorMetrics(iKegeratorMetrics)
  {
    mKegeratorMetrics.attach(this);
  }

  ConnectionHandler(const ConnectionHandler &iConnectionHandler)
    : mKegeratorMetrics(iConnectionHandler.mKegeratorMetrics)
  {
    mKegeratorMetrics.attach(this);
  }

  ~ConnectionHandler()
  {
    mKegeratorMetrics.detach(this);
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
      wSender->send(mKegeratorMetrics.dataString());
    }
  }

private:
  void handleMessage(const std::string &iPayload)
  {
    if (iPayload == "get_status")
    {
      update(mKegeratorMetrics);
    }
  }

  std::weak_ptr< ISender > mSender;
  const KegeratorMetrics &mKegeratorMetrics;
};
