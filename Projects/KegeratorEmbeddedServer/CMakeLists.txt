SET(this_target KegeratorEmbeddedServer)

INCLUDE_DIRECTORIES(
  ${Boost_INCLUDE_DIRS}
  ${Internal_INCLUDE}
)

SET(${this_target}_HEADER_FILES
  src/Ads1115.h
  src/Bmp180.h
  src/ActiveObject.h
  src/ThreadPool.h
  src/SamplingTasksRegistrar.h
  src/InterruptTasksRegistrar.h
  src/KegeratorMetrics.h
  src/KegeratorSamplingTasks.h
  src/KegeratorInterruptTasks.h
  src/IObserver.h
  src/Subject.h
  src/TcpServer/ISender.h
  src/TcpServer/TcpServer.h
  src/TcpServer/TcpConnection.h
  src/TcpServer/WebSocketConnection.h
  src/ConnectionHandler.h
  src/Operations.h
  src/FilteredValue.h
  src/MovingAveragedValue.h
  src/Statement.h
  src/sqlite/sqlite3.h
)

SET( ${this_target}_SOURCE_FILES
  src/main.cpp
  src/PressureAndTemperatureSampling.cpp
  src/Co2MassSampling.cpp
  src/FlowMeterSensing.cpp
  src/Statement.cpp
  src/KegeratorMetrics.cpp
  src/sqlite/sqlite3.c
) 
 
LIST(APPEND ${this_target}_SOURCE_FILES ${${this_target}_HEADER_FILES})

add_executable(${this_target} ${${this_target}_SOURCE_FILES})
target_link_libraries(${this_target} ${Boost_LIBRARIES})
target_link_libraries(${this_target} InterIntegratedCircuit)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
target_link_libraries(${this_target} pthread dl)
ENDIF()
