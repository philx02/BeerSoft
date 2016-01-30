#include "ActiveObject.h"
#include "ThreadPool.h"
#include "KegaratorSamplingTasks.h"
#include "TcpServer/TcpServer.h"
#include "TcpServer/WebSocketConnection.h"
#include "ConnectionHandler.h"

#include <boost/asio.hpp>

KegaratorSamplingTasks & KegaratorSamplingTasks::getInstance()
{
  static KegaratorSamplingTasks wKegaratorSamplingTasks;
  return wKegaratorSamplingTasks;
}

int main(int argc, char *argv[])
{
  DataActiveObject< KegaratorMetrics > wKegaratorMetrics = KegaratorMetrics();

  boost::asio::io_service wIoService;
  auto wWebSocketServer = createTcpServer< WebSocketConnection >(wIoService, ConnectionHandler(wKegaratorMetrics.getConstInternal()), 8000);

  ThreadPool wThreadPool(KegaratorSamplingTasks::getInstance().size());
  auto wKegaratorMetricsThread = std::thread([&]() { wKegaratorMetrics.run(); });
  
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

  return 0;
}
