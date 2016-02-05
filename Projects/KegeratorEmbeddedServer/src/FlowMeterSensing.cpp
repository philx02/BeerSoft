#include "Operations.h"
#include "KegeratorInterruptTasks.h"

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
  pollfd mPollFd;
  size_t mKegIndex;
};

static FlowMeterSensing sFlowMeterSensingKeg0(std::getenv("FLOW_METER_0_GPIO_PATH"), 0);
static FlowMeterSensing sFlowMeterSensingKeg1(std::getenv("FLOW_METER_1_GPIO_PATH"), 1);
