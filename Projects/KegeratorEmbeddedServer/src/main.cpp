#include "ActiveObject.h"
#include "ThreadPool.h"
#include "KegeratorSamplingTasks.h"
#include "KegeratorInterruptTasks.h"
#include "TcpServer/TcpServer.h"
#include "TcpServer/WebSocketConnection.h"
#include "ConnectionHandler.h"
#include "KegeratorMetrics.h"

#include <boost/asio.hpp>

template<>
KegaratorSamplingTasks & KegaratorSamplingTasks::getInstance()
{
  static KegaratorSamplingTasks wKegaratorSamplingTasks;
  return wKegaratorSamplingTasks;
}

template<>
KegaratorInterruptTasks & KegaratorInterruptTasks::getInstance()
{
  static KegaratorInterruptTasks wKegaratorInterruptTasks;
  return wKegaratorInterruptTasks;
}

class ConsoleOutput : public IObserver< KegaratorMetrics >
{
public:
  virtual void update(const KegaratorMetrics &iMetrics)
  {
    std::cout << iMetrics.dataString() << std::endl;
  }
};

int main(int argc, char *argv[])
{

  DataActiveObject< KegaratorMetrics > wKegaratorMetrics = KegaratorMetrics(argc == 2 ? argv[1] : "");

  ConsoleOutput wConsoleOutput;
  //wKegaratorMetrics.getConstInternal().attach(&wConsoleOutput);

  boost::asio::io_service wIoService;
  auto wWebSocketServer = createTcpServer< WebSocketConnection >(wIoService, ConnectionHandler(wKegaratorMetrics.getConstInternal()), 8000);
  std::thread wTcpServerThread([&]() { wIoService.run(); });

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

  wTcpServerThread.join();

  return 0;
}
