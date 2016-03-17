#pragma once

#include "Subject.h"
#include "HeaterControl.h"

#include <memory>

class BrewControl : public Subject< BrewControl >
{
public:
  BrewControl(std::unique_ptr< HeaterControl > &&iHeaterControl)
    : mHeaterControl(std::move(iHeaterControl))
  {
  }

  BrewControl(BrewControl &&iBrewControl)
    : mHeaterControl(std::move(iBrewControl.mHeaterControl))
  {
  }

  void setTemperatureCommand(double iTemperature)
  {
    mHeaterControl->setTemperatureCommand(iTemperature);
    Subject< BrewControl >::notify(*this);
  }

  void setActualTemperature(double iTemperature)
  {
    mHeaterControl->setActualTemperature(iTemperature);
    Subject< BrewControl >::notify(*this);
  }

  std::string dataString() const
  {
    return std::to_string(mHeaterControl->getTemperatureCommand()) + "," + std::to_string(mHeaterControl->getActualTemperature()) + "," + std::to_string(mHeaterControl->getActualDutyCycle());
  }

private:
  std::unique_ptr< HeaterControl > mHeaterControl;
};
