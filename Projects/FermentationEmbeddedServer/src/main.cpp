#include "PeriodicTimer.h"
#include "CalibratedSensor.h"
#include "Ads1115.h"
#include "Max6675.h"

#include <memory>
#include <iostream>
#include <thread>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>

namespace bpo = boost::program_options;

int main(int argc, char *argv[])
{
  bpo::options_description wDescriptions("Allowed options");
  wDescriptions.add_options()
    ("help,h", "produce help message")
    ("bus,b", bpo::value< std::string >()->default_value("/dev/i2c-2"), "i2c bus")
    ("address,a", bpo::value< uint8_t >()->default_value(0x68), "address of the i2c device")
    ("density_calibration,d", bpo::value< std::string >()->required(), "density calibration file")
    ("density_channel", bpo::value< uint8_t >()->default_value(0), "density channel, from 0 to 3")
    ("density_resolution", bpo::value< uint8_t >()->default_value(2), "density resolution, from 0 to 3")
    ("density_gain", bpo::value< uint8_t >()->default_value(3), "density gain, from 0 to 3")
    ("spi_device,s", bpo::value< std::string >()->default_value("/dev/spidev32766.0"), "spi device")
    ("spi_speed", bpo::value< size_t >()->default_value(500000), "spi speed (hz)")
    ;

  bpo::variables_map wOptions;
  bpo::store(bpo::parse_command_line(argc, argv, wDescriptions), wOptions);

  if (wOptions.count("help"))
  {
    std::cout << wDescriptions << std::endl;
    exit(1);
  }

  bpo::notify(wOptions);

  CalibratedSensor< Ads1115 > wDensitySensor(wOptions["density_calibration"].as< std::string >().c_str(), std::make_unique< Ads1115 >(wOptions["bus"].as< std::string >().c_str(), static_cast< Ads1115::eGainCode >(wOptions["density_gain"].as< uint8_t >())));
  Max6675 wTemperatureSensor(wOptions["spi_device"].as< std::string >().c_str(), wOptions["spi_speed"].as< size_t >());
  
  boost::asio::io_service wIoService;
  auto wTimer = createPeriodicTimer(wIoService, [&]()
  {
    std::cout << std::chrono::duration_cast< std::chrono::seconds >(std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << "," << wDensitySensor.sampleRaw() << "," << wDensitySensor.sampleCalibrated();
    std::cout << "," << wTemperatureSensor.read();
    std::cout << std::endl;
    return true;
  }, std::chrono::seconds(1));

  wTimer->start();
  wIoService.run();

  return 0;
}