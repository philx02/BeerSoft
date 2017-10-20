#include "TemperatureSampling.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <thread>

#include <iostream>

namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;
namespace bip = boost::asio::ip;

int main(int argc, char *argv[])
{
  bfs::path wPath;
  bfs::path wCalibration;
  std::string wRemoteHost;
  std::string wRemotePort;
  size_t wPeriod = 0;
  bpo::options_description wDescriptions("Allowed options");
  wDescriptions.add_options()
    ("help,h", "produce help message")
    ("path,p", bpo::value(&wPath)->required(), "Mcp3424 device path")
    ("calibration,c", bpo::value(&wCalibration)->required(), "Path to calibration file")
    ("remote_host,r", bpo::value(&wRemoteHost)->required(), "Remote host")
    ("remote_port,o", bpo::value(&wRemotePort)->required(), "Remote port")
    ("period_milliseconds,m", bpo::value(&wPeriod)->required(), "Sampling period in milliseconds");

  bpo::variables_map wOptions;
  bpo::store(bpo::parse_command_line(argc, argv, wDescriptions), wOptions);

  if (wOptions.count("help"))
  {
    std::cout << wDescriptions << std::endl;
    exit(1);
  }

  bpo::notify(wOptions);

  if (!bfs::exists(wPath))
  {
    std::cerr << "path not found" << std::endl;
    exit(1);
  }
  if (!bfs::exists(wCalibration))
  {
    std::cerr << "calibration not found" << std::endl;
    exit(1);
  }

  TemperatureSampling wSampling(wPath.string().c_str(), wCalibration.string().c_str());
  boost::asio::io_service wIoService;
  bip::udp::socket wSocket(wIoService, bip::udp::v4());
  bip::udp::endpoint wHost = [&]()
  {
    bip::udp::resolver wResolver(wIoService);
    bip::udp::resolver::query wQuery(bip::udp::v4(), wRemoteHost, wRemotePort);
    return *wResolver.resolve(wQuery);
  }();

  while (true)
  {
    auto wSample = wSampling.sample();
    wSocket.send_to(boost::asio::buffer(std::to_string(wSample)), wHost);
    std::this_thread::sleep_for(std::chrono::milliseconds(wPeriod));
  }

  return 0;
}
