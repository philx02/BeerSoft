#pragma once

#include "TcpServer/ISender.h"
#include "KegeratorMetrics.h"

class ConnectionHandler : public IObserver< KegaratorMetrics >
{
public:
  ConnectionHandler(const KegaratorMetrics &iKegaratorMetrics)
    : mKegaratorMetrics(iKegaratorMetrics)
  {
    mKegaratorMetrics.attach(this);
  }

  ~ConnectionHandler()
  {
    mKegaratorMetrics.detach(this);
  }

  void setSender(const std::shared_ptr< ISender > &iSender)
  {
    mSender = iSender;
  }

  void operator()(const std::string &iPayload)
  {
    handleMessage(iPayload);
  }

  void update(const KegaratorMetrics &iMetrics)
  {
    auto wSender = mSender.lock();
    if (wSender != nullptr)
    {
      wSender->send(mKegaratorMetrics.dataString());
    }
  }

private:
  void handleMessage(const std::string &iPayload)
  {
    if (iPayload == "get_status")
    {
      update(mKegaratorMetrics);
    }
  }

  std::weak_ptr< ISender > mSender;
  const KegaratorMetrics &mKegaratorMetrics;
};
