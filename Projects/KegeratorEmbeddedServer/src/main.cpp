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
KegeratorSamplingTasks & KegeratorSamplingTasks::getInstance()
{
  static KegeratorSamplingTasks wKegeratorSamplingTasks;
  return wKegeratorSamplingTasks;
}

template<>
KegeratorInterruptTasks & KegeratorInterruptTasks::getInstance()
{
  static KegeratorInterruptTasks wKegeratorInterruptTasks;
  return wKegeratorInterruptTasks;
}

class ConsoleOutput : public IObserver< KegeratorMetrics >
{
public:
  virtual void update(const KegeratorMetrics &iMetrics)
  {
    std::cout << iMetrics.dataString() << std::endl;
  }
};

int main(int argc, char *argv[])
{

  DataActiveObject< KegeratorMetrics > wKegeratorMetrics = KegeratorMetrics(argc == 2 ? argv[1] : "");

  ConsoleOutput wConsoleOutput;
  //wKegeratorMetrics.getConstInternal().attach(&wConsoleOutput);

  boost::asio::io_service wIoService;
  auto wWebSocketServer = createTcpServer< WebSocketConnection >(wIoService, ConnectionHandler(wKegeratorMetrics.getConstInternal()), 8000);
  std::thread wTcpServerThread([&]() { wIoService.run(); });

  ThreadPool wThreadPool(KegeratorSamplingTasks::getInstance().size());
  auto wKegeratorMetricsThread = std::thread([&]() { wKegeratorMetrics.run(); });

  std::vector< std::thread > wInterruptThreads;
  for (auto &&wInterruptTask : KegeratorInterruptTasks::getInstance())
  {
    wInterruptThreads.emplace_back([&]() { wInterruptTask(wKegeratorMetrics); });
  }
  
  while (true)
  {
    for (auto &&wSamplingTask : KegeratorSamplingTasks::getInstance())
    {
      wThreadPool.enqueue([&]() { wSamplingTask(wKegeratorMetrics); });
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  wKegeratorMetrics.stop();
  wKegeratorMetricsThread.join();

  for (auto &&wInterruptThread : wInterruptThreads)
  {
    wInterruptThread.join();
  }

  wTcpServerThread.join();

  return 0;
}
