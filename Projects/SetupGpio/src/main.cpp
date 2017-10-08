#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;

int main(int argc, char *argv[])
{
  bfs::path wPath;
  std::string wActivationLevel;
  std::string wDirection;
  std::string wEdge;
  size_t wId;
  int wInitialValue = -1;
  bpo::options_description wDescriptions("Allowed options");
  wDescriptions.add_options()
    ("help,h", "produce help message")
    ("path,p", bpo::value(&wPath)->default_value("/sys/class/gpio"), "Path to gpio folder")
    ("id,i", bpo::value(&wId)->required(), "Gpio ID")
    ("activation_level,a", bpo::value(&wActivationLevel)->required(), "Activation level (low or high)")
    ("edge,e", bpo::value(&wEdge), "Edge")
    ("initial_value,v", bpo::value(&wInitialValue), "Initial Value")
    ("direction,d", bpo::value(&wDirection)->required(), "Direction (out or in)");
  
  bpo::variables_map wOptions;
  bpo::store(bpo::parse_command_line(argc, argv, wDescriptions), wOptions);

  if (wOptions.count("help"))
  {
    std::cout << wDescriptions << std::endl;
    exit(1);
  }

  bpo::notify(wOptions);

  {
    std::ofstream wExport((wPath / "export").string().c_str());
    wExport << wId << std::endl;
  }

  std::string wGpioName = std::string("gpio") + std::to_string(wId);
  {
    std::ofstream wGpioActivationLevel((wPath / wGpioName / "active_low").string().c_str());
    std::ofstream wGpioDirection((wPath / wGpioName / "direction").string().c_str());
    std::ofstream wGpioEdge((wPath / wGpioName / "edge").string().c_str());
    std::ofstream wGpioValue((wPath / wGpioName / "value").string().c_str());
    wGpioActivationLevel << (wActivationLevel == "low" ? 1 : 0) << std::endl;
    wGpioDirection << wDirection << std::endl;
    if (wEdge != "")
    {
      wGpioEdge << wEdge << std::endl;
    }
    if (wInitialValue != -1)
    {
      wGpioValue << wInitialValue << std::endl;
    }
  }

  return 0;
}
