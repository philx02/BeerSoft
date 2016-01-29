SET(this_target InterIntegratedCircuit)

SET(${this_target}_HEADER_FILES
  src/DllSwitch.h
  src/I2CDevice.h
  src/smbus.h
)

SET( ${this_target}_SOURCE_FILES
  src/I2CDevice.cpp
  src/smbus.cpp
)
 
LIST(APPEND ${this_target}_SOURCE_FILES ${${this_target}_HEADER_FILES})

add_library(${this_target} SHARED ${${this_target}_SOURCE_FILES})
