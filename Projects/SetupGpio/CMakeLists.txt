SET(this_target SetupGpio)

INCLUDE_DIRECTORIES(
  ${Boost_INCLUDE_DIRS}
  ${Internal_INCLUDE}
)

SET( ${this_target}_SOURCE_FILES
  src/main.cpp
) 
 
LIST(APPEND ${this_target}_SOURCE_FILES ${${this_target}_HEADER_FILES})

add_executable(${this_target} ${${this_target}_SOURCE_FILES})
target_link_libraries(${this_target} ${Boost_LIBRARIES})
