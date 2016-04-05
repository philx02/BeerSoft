#pragma once

#include "Subject.h"
#include "HeaterControl.h"
#include "PumpControl.h"

#include <memory>

class BrewControl : public Subject< BrewControl >, public IObserver< PumpControl >
{
public:
  BrewControl(std::unique_ptr< HeaterControl > &&iHeaterControl, std::unique_ptr< PumpControl > &&iPumpControl)
    : mHeaterControl(std::move(iHeaterControl))
    , mPumpControl(std::move(iPumpControl))
  {
    mPumpControl->attach(this);
  }

  BrewControl(BrewControl &&iBrewControl)
    : mHeaterControl(std::move(iBrewControl.mHeaterControl))
    , mPumpControl(std::move(iBrewControl.mPumpControl))
  {
    mPumpControl->detach(&iBrewControl);
    mPumpControl->attach(this);
  }

  void setTemperatureCommand(double iTemperature) const
  {
    mHeaterControl->setTemperatureCommand(iTemperature);
    Subject< BrewControl >::notify(*this);
  }

  void setActualTemperature(double iTemperature) const
  {
    mHeaterControl->setActualTemperature(iTemperature);
    Subject< BrewControl >::notify(*this);
  }

  void setHeaterMode(size_t iMode) const
  {
    mHeaterControl->setMode(static_cast< HeaterControl::eMode >(iMode));
    Subject< BrewControl >::notify(*this);
  }

  void setPumpDuration(size_t iSeconds) const
  {
    mPumpControl->setTimerLimit(std::chrono::seconds(iSeconds));
    Subject< BrewControl >::notify(*this);
  }

  void setPumpMode(size_t iMode) const
  {
    mPumpControl->setMode(static_cast< PumpControl::eMode >(iMode));
    Subject< BrewControl >::notify(*this);
  }

  virtual void update(const PumpControl &iPumpControl)
  {
    Subject< BrewControl >::notify(*this);
  }

  std::string dataString() const
  {
    return std::to_string(mHeaterControl->getTemperatureCommand()) + "," + std::to_string(mHeaterControl->getActualTemperature()) + "," + std::to_string(mHeaterControl->getActualDutyCycle()) + "," + std::to_string(mHeaterControl->getMode()) + "," + std::to_string(mPumpControl->getMode()) + "," + std::to_string(mPumpControl->getRemainingTime().count());
  }

private:
  std::unique_ptr< HeaterControl > mHeaterControl;
  std::unique_ptr< PumpControl > mPumpControl;
};
