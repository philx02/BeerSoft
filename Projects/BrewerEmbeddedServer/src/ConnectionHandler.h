#pragma once

#include "TcpServer/ISender.h"
#include "BrewControl.h"

class ConnectionHandler : public IObserver< BrewControl >
{
public:
  ConnectionHandler(const BrewControl &iBrewControl)
    : mBrewControl(iBrewControl)
  {
    mBrewControl.attach(this);
  }

  ConnectionHandler(const ConnectionHandler &iConnectionHandler)
    : mBrewControl(iConnectionHandler.mBrewControl)
  {
    mBrewControl.attach(this);
  }

  ~ConnectionHandler()
  {
    mBrewControl.detach(this);
  }

  void setSender(const std::shared_ptr< ISender > &iSender)
  {
    mSender = iSender;
  }

  void operator()(const std::string &iPayload)
  {
    handleMessage(iPayload);
  }

  void update(const BrewControl &iMetrics)
  {
    auto wSender = mSender.lock();
    if (wSender != nullptr)
    {
      //wSender->send(mBrewControl.dataString());
    }
  }

private:
  void handleMessage(const std::string &iPayload)
  {
    if (iPayload == "get_status")
    {
      update(mBrewControl);
    }
  }

  std::weak_ptr< ISender > mSender;
  const BrewControl &mBrewControl;
};
