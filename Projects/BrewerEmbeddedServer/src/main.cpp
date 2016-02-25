#include "ActiveObject.h"
#include "ThreadPool.h"
#include "BrewControlSamplingTasks.h"
#include "TcpServer/TcpServer.h"
#include "TcpServer/WebSocketConnection.h"
#include "ConnectionHandler.h"
#include "BrewControl.h"

#include <boost/asio.hpp>

template<>
BrewControlSamplingTasks & BrewControlSamplingTasks::getInstance()
{
  static BrewControlSamplingTasks wBrewControlSamplingTasks;
  return wBrewControlSamplingTasks;
}

int main(int argc, char *argv[])
{
  DataActiveObject< BrewControl > wBrewControl = BrewControl();

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