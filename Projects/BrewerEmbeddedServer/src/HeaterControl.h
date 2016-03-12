#pragma once

#include <fstream>

class HeaterControl
{
public:
  HeaterControl(const char *iPwmGpio)
    : mPwmGpio(iPwmGpio)
    , mTemperatureCommand(0)
    , mActualTemperature(0)
  {
  }

  HeaterControl(HeaterControl &&iHeaterControl)
    : mPwmGpio(std::move(iHeaterControl.mPwmGpio))
    , mTemperatureCommand(iHeaterControl.mTemperatureCommand)
    , mActualTemperature(iHeaterControl.mActualTemperature)
  {
  }

  void setTemperatureCommand(double iTemperature)
  {
    mTemperatureCommand = iTemperature;
  }

  void setActualTemperature(double iTemperature)
  {
    mActualTemperature = iTemperature;
  }

  double getTemperatureCommand() const
  {
    return mTemperatureCommand;
  }

  double getActualTemperature() const
  {
    return mActualTemperature;
  }

private:
  std::ofstream mPwmGpio;
  double mTemperatureCommand;
  double mActualTemperature;
};
