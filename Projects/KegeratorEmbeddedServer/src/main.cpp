#include "ActiveObject.h"
#include "ThreadPool.h"
#include "KegeratorSamplingTasks.h"
#include "KegeratorInterruptTasks.h"
#include "TcpServer/TcpServer.h"
#include "TcpServer/WebSocketConnection.h"
#include "ConnectionHandler.h"
#include "KegeratorMetrics.h"

#include <iostream>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;

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

int main(int argc, char *argv[])
{
  bpo::options_description wDesc("Allowed options");
  wDesc.add_options()
    ("help", "produce help message")
    ("data,d", bpo::value< bfs::path >(), "Directory containing the runtime data")
    ("port,p", bpo::value< uint16_t >(), "Websocket port")
    ;

  bpo::variables_map wConsoleArgs;
  bpo::store(bpo::parse_command_line(argc, argv, wDesc), wConsoleArgs);
  bpo::notify(wConsoleArgs);

  if (wConsoleArgs.count("help") == 1)
  {
    std::cout << wDesc << std::endl;
    exit(1);
  }

  DataActiveObject< KegeratorMetrics > wKegeratorMetrics = KegeratorMetrics(wConsoleArgs["data"].as< bfs::path >());
  
  boost::asio::io_service wIoService;
  auto wWebSocketServer = createTcpServer< WebSocketConnection >(wIoService, ConnectionHandler(wKegeratorMetrics), wConsoleArgs["port"].as< uint16_t >());
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
    wKegeratorMetrics.pushDataAccess([&](KegeratorMetrics &iMetrics)
    {
      iMetrics.notifyAll();
    });
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
