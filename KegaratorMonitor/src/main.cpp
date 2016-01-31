#include "ActiveObject.h"
#include "ThreadPool.h"
#include "KegaratorSamplingTasks.h"
#include "KegaratorInterruptTasks.h"
#include "TcpServer/TcpServer.h"
#include "TcpServer/WebSocketConnection.h"
#include "ConnectionHandler.h"

#include <boost/asio.hpp>

KegaratorSamplingTasks & KegaratorSamplingTasks::getInstance()
{
  static KegaratorSamplingTasks wKegaratorSamplingTasks;
  return wKegaratorSamplingTasks;
}

KegaratorInterruptTasks & KegaratorInterruptTasks::getInstance()
{
  static KegaratorInterruptTasks wKegaratorInterruptTasks;
  return wKegaratorInterruptTasks;
}

int main(int argc, char *argv[])
{
  DataActiveObject< KegaratorMetrics > wKegaratorMetrics = KegaratorMetrics();

  boost::asio::io_service wIoService;
  auto wWebSocketServer = createTcpServer< WebSocketConnection >(wIoService, ConnectionHandler(wKegaratorMetrics.getConstInternal()), 8000);

  ThreadPool wThreadPool(KegaratorSamplingTasks::getInstance().size());
  auto wKegaratorMetricsThread = std::thread([&]() { wKegaratorMetrics.run(); });

  std::vector< std::thread > wInterruptThreads;
  for (auto &&wInterruptTask : KegaratorInterruptTasks::getInstance())
  {
    wInterruptThreads.emplace_back([&]() { wInterruptTask(wKegaratorMetrics); });
  }
  
  while (true)
  {
    for (auto &&wSamplingTask : KegaratorSamplingTasks::getInstance())
    {
      wThreadPool.enqueue([&]() { wSamplingTask(wKegaratorMetrics); });
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  wKegaratorMetrics.stop();
  wKegaratorMetricsThread.join();

  for (auto &&wInterruptThread : wInterruptThreads)
  {
    wInterruptThread.join();
  }

  return 0;
}
