SET(this_target FermentationEmbeddedServer)

INCLUDE_DIRECTORIES(
  ${Boost_INCLUDE_DIRS}
  ${Internal_INCLUDE}
)

SET(${this_target}_HEADER_FILES
  src/Ads1115.h
  src/Max6675.h
  src/PeriodicTimer.h
  src/CalibratedSensor.h
)

SET( ${this_target}_SOURCE_FILES
  src/main.cpp
) 

#SET_SOURCE_FILES_PROPERTIES(${this_target}_HEADER_FILES
#                            PROPERTIES HEADER_FILE_ONLY TRUE)
 
LIST(APPEND ${this_target}_SOURCE_FILES ${${this_target}_HEADER_FILES})

add_executable(${this_target} ${${this_target}_SOURCE_FILES})
target_link_libraries(${this_target} ${Boost_LIBRARIES})
target_link_libraries(${this_target} InterIntegratedCircuit)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
target_link_libraries(${this_target} pthread)
ENDIF()
