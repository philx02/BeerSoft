#include "Operations.h"
#include "KegaratorInterruptTasks.h"

class FlowMeterSensing
{
public:
  FlowMeterSensing(const char *iInputGpio, size_t iKegIndex)
    : mGpioFd(openGpio(iInputGpio))
    , mPollFd(openSwitchListener(mGpioFd))
    , mKegIndex(iKegIndex)
  {
    KegaratorInterruptTasks::getInstance().addInterruptTask([&](DataActiveObject< KegaratorMetrics > &iKegaratorMetrics)
    {
      while (true)
      {
        auto wGpioValue = readGpio(mPollFd, mGpioFd);
        if (wGpioValue)
        {
          iKegaratorMetrics.dataPush([&](KegaratorMetrics &iMetrics)
          {
            iMetrics.pulseKeg(mKegIndex);
          });
        }
      }
    });
  }

private:
  int mGpioFd;
  int mPollFd;
  size_t mKegIndex;
};

static FlowMeterSensing sFlowMeterSensingKeg0("/dev/i2c-1", 0);
static FlowMeterSensing sFlowMeterSensingKeg1("/dev/i2c-1", 1);
