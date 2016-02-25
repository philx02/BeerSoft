#pragma once

#include "Subject.h"

class BrewControl : public Subject< BrewControl >
{
public:
  BrewControl()
  {
  }

  BrewControl(BrewControl &&iBrewControl)
    : mTemperatureCommand(iBrewControl.mTemperatureCommand)
    , mActualTemperature(iBrewControl.mActualTemperature)
  {
  }

  void setActualTemperature(double iTemperature)
  {
  }

private:
  double mTemperatureCommand;
  double mActualTemperature;
};
