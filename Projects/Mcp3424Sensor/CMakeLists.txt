SET(this_target Mcp3424Sensor)

INCLUDE_DIRECTORIES(
  ${Boost_INCLUDE_DIRS}
  ${Internal_INCLUDE}
)

SET(${this_target}_HEADER_FILES
  src/Mcp3424.h
  src/TemperatureSampling.h
)

SET( ${this_target}_SOURCE_FILES
  src/main.cpp
  src/TemperatureSampling.cpp
) 
 
LIST(APPEND ${this_target}_SOURCE_FILES ${${this_target}_HEADER_FILES})

add_executable(${this_target} ${${this_target}_SOURCE_FILES})
target_link_libraries(${this_target} ${Boost_LIBRARIES})
target_link_libraries(${this_target} InterIntegratedCircuit)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
target_link_libraries(${this_target} pthread)
ENDIF()
