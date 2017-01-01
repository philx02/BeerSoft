#include <iostream>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>

namespace ba = boost::accumulators;

int main(int argc, char *argv[])
{
  size_t wInterval = std::strtoul(argv[1], nullptr, 10);
  std::string wLine;
  std::vector< std::string > wSplit;
  std::vector< std::pair< size_t, double > > wData;
  ba::accumulator_set< double, ba::stats< ba::tag::rolling_mean, ba::tag::count > > wAcc(ba::tag::rolling_window::window_size = wInterval);
  size_t wSegmentTimestamp;
  while (std::getline(std::cin, wLine))
  {
    boost::split(wSplit, wLine, boost::is_any_of(","));
    if (ba::count(wAcc) % wInterval == 0)
    {
      if (ba::count(wAcc) > 0)
      {
        wData.emplace_back(wSegmentTimestamp, ba::rolling_mean(wAcc));
      }
      wSegmentTimestamp = std::strtoul(wSplit[0].c_str(), nullptr, 10);
    }
    wAcc(std::strtod(wSplit[2].c_str(), nullptr));
  }

  for (auto &&wEntry : wData)
  {
    std::cout << wEntry.first << "," << wEntry.second << "\n";
  }

  return 0;
}