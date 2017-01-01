#include "PeriodicTimer.h"
#include "CalibratedSensor.h"
#include "Mcp3424.h"

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
    ("temperature_calibration,t", bpo::value< std::string >()->required(), "temperature calibration file")
    ("temperature_channel", bpo::value< uint8_t >()->default_value(1), "temperature channel, from 0 to 3")
    ("temperature_resolution", bpo::value< uint8_t >()->default_value(2), "temperature resolution, from 0 to 3")
    ("temperature_gain", bpo::value< uint8_t >()->default_value(3), "temperature gain, from 0 to 3")
    ;

  bpo::variables_map wOptions;
  bpo::store(bpo::parse_command_line(argc, argv, wDescriptions), wOptions);

  if (wOptions.count("help"))
  {
    std::cout << wDescriptions << std::endl;
    exit(1);
  }

  bpo::notify(wOptions);

  auto wMcp3424 = std::make_shared< I2CDevice >(wOptions["bus"].as< std::string >().c_str(), wOptions["address"].as< uint8_t >());

  CalibratedSensor< Mcp3424Channel > wDensitySensor(wOptions["density_calibration"].as< std::string >().c_str(), std::make_unique< Mcp3424Channel >(wMcp3424, wOptions["density_channel"].as< uint8_t >(), static_cast< Mcp3424Channel::Resolution >(wOptions["density_resolution"].as< uint8_t >()), static_cast< Mcp3424Channel::Gain >(wOptions["density_gain"].as< uint8_t >())));
  //CalibratedSensor< Mcp3424Channel > wTemperatureSensor(wOptions["temperature_calibration"].as< std::string >().c_str(), std::make_unique< Mcp3424Channel >(wMcp3424, wOptions["temperature_channel"].as< uint8_t >(), static_cast< Mcp3424Channel::Resolution >(wOptions["temperature_resolution"].as< uint8_t >()), static_cast< Mcp3424Channel::Gain >(wOptions["temperature_gain"].as< uint8_t >())));
  
  boost::asio::io_service wIoService;
  auto wTimer = createPeriodicTimer(wIoService, [&]()
  {
    std::cout << std::chrono::duration_cast< std::chrono::seconds >(std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << "," << wDensitySensor.sampleRaw() << "," << wDensitySensor.sampleCalibrated();
    //std::cout << "," << wTemperatureSensor.sampleRaw() << "," << wTemperatureSensor.sampleCalibrated();
    std::cout << std::endl;
    return true;
  }, std::chrono::seconds(1));

  wTimer->start();
  wIoService.run();

  return 0;
}