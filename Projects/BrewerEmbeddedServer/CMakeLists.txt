SET(this_target BrewerEmbeddedServer)

INCLUDE_DIRECTORIES(
  ${Boost_INCLUDE_DIRS}
  ${Internal_INCLUDE}
)

SET(${this_target}_HEADER_FILES
  src/Ads1115.h
  src/ActiveObject.h
  src/ThreadPool.h
  src/SamplingTasksRegistrar.h
  src/BrewControl.h
  src/BrewControlSamplingTasks.h
  src/IObserver.h
  src/Subject.h
  src/TcpServer/ISender.h
  src/TcpServer/TcpServer.h
  src/TcpServer/TcpConnection.h
  src/TcpServer/WebSocketConnection.h
  src/ConnectionHandler.h
  src/HeaterControl.h
  src/PumpControl.h
  src/PeriodicTimer.h
)

SET( ${this_target}_SOURCE_FILES
  src/main.cpp
  src/TemperatureSampling.cpp
) 
 
LIST(APPEND ${this_target}_SOURCE_FILES ${${this_target}_HEADER_FILES})

add_executable(${this_target} ${${this_target}_SOURCE_FILES})
target_link_libraries(${this_target} ${Boost_LIBRARIES})
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
target_link_libraries(${this_target} pthread)
ENDIF()
