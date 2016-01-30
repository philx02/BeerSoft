SET(this_target KegaratorMonitor)

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
  src/KegaratorMetrics.h
  src/KegaratorSamplingTasks.h
  src/IObserver.h
  src/Subject.h
  src/TcpServer/ISender.h
  src/TcpServer/TcpServer.h
  src/TcpServer/TcpConnection.h
  src/TcpServer/WebSocketConnection.h
  src/ConnectionHandler.h
)

SET( ${this_target}_SOURCE_FILES
  src/main.cpp
  src/PressureAndTemperatureSampling.cpp
) 

#SET_SOURCE_FILES_PROPERTIES(${this_target}_HEADER_FILES
#                            PROPERTIES HEADER_FILE_ONLY TRUE)
 
LIST(APPEND ${this_target}_SOURCE_FILES ${${this_target}_HEADER_FILES})

add_executable(${this_target} ${${this_target}_SOURCE_FILES})
target_link_libraries(KegaratorMonitor ${Boost_LIBRARIES})
target_link_libraries(KegaratorMonitor InterIntegratedCircuit)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
ENDIF()
