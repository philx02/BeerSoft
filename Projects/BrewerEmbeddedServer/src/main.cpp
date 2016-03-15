#include "ActiveObject.h"
#include "ThreadPool.h"
#include "BrewControlSamplingTasks.h"
#include "TcpServer/TcpServer.h"
#include "TcpServer/WebSocketConnection.h"
#include "ConnectionHandler.h"
#include "HeaterControl.h"
#include "BrewControl.h"

#include <boost/asio.hpp>

#include <memory>

template<>
BrewControlSamplingTasks & BrewControlSamplingTasks::getInstance()
{
  static BrewControlSamplingTasks wBrewControlSamplingTasks;
  return wBrewControlSamplingTasks;
}

int main(int argc, char *argv[])
{
  DataActiveObject< BrewControl > wBrewControl = BrewControl(std::make_unique< HeaterControl >(std::getenv("PWM_GPIO_DEVICE"), boost::posix_time::milliseconds(10), boost::posix_time::seconds(2), 20));

  boost::asio::io_service wIoService;
  auto wWebSocketServer = createTcpServer< WebSocketConnection >(wIoService, ConnectionHandler(wBrewControl.getConstInternal()), 8000);
  std::thread wTcpServerThread([&]() { wIoService.run(); });

  ThreadPool wThreadPool(BrewControlSamplingTasks::getInstance().size());
  auto wBrewControlThread = std::thread([&]() { wBrewControl.run(); });
  
  while (true)
  {
    for (auto &&wSamplingTask : BrewControlSamplingTasks::getInstance())
    {
      wThreadPool.enqueue([&]() { wSamplingTask(wBrewControl); });
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  wBrewControl.stop();
  wBrewControlThread.join();

  wTcpServerThread.join();

  return 0;
}
