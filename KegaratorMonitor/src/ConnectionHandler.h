#pragma once

#include "TcpServer/ISender.h"
#include "KegaratorMetrics.h"

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
  }

private:
  void handleMessage(const std::string &iPayload)
  {
  }

  std::weak_ptr< ISender > mSender;
  const KegaratorMetrics &mKegaratorMetrics;
};
