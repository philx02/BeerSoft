#pragma once

#include "TcpServer/ISender.h"
#include "BrewControl.h"

#include <boost/algorithm/string.hpp>

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
      wSender->send(mBrewControl.dataString());
    }
  }

private:
  void handleMessage(const std::string &iPayload)
  {
    if (iPayload == "get_status")
    {
      update(mBrewControl);
    }
    else
    {
      std::vector< std::string > wSplit;
      boost::split(wSplit, iPayload, boost::is_any_of(","));
      if (wSplit.size() == 2 && wSplit[0] == "set_temperature_command")
      {
        mBrewControl.setTemperatureCommand(std::strtod(wSplit[1].c_str(), nullptr));
      }
      else if (wSplit.size() == 2 && wSplit[0] == "set_heater_mode")
      {
        mBrewControl.setHeaterMode(std::strtoul(wSplit[1].c_str(), nullptr, 10));
      }
      else if (wSplit.size() == 2 && wSplit[0] == "set_pump_mode")
      {
        mBrewControl.setPumpMode(std::strtoul(wSplit[1].c_str(), nullptr, 10));
      }
      else if (wSplit.size() == 2 && wSplit[0] == "set_pump_until_command")
      {
        mBrewControl.setPumpDuration(std::strtoul(wSplit[1].c_str(), nullptr, 10));
      }
    }
  }

  std::weak_ptr< ISender > mSender;
  const BrewControl &mBrewControl;
};
