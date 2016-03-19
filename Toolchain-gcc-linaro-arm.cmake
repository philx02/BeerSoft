# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR arm)

# specify the cross compiler
SET(CMAKE_C_COMPILER   $ENV{HOME}/rasp_tools/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER $ENV{HOME}/rasp_tools/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-c++)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH
  $ENV{HOME}/rasp_tools/rootfs
  $ENV{HOME}/rasp_tools/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf
  /media/sf_workspace/boost_1_55_0/arm
)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

