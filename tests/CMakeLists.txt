cmake_minimum_required(VERSION 3.22)
project(tests)

Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.1 # or a later release
  )
FetchContent_MakeAvailable(Catch2)

add_executable( tests
  date_tests.cpp
  time_duration_tests.cpp
  utc_date_time_tests.cpp
  )

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED)
message(STATUS "Boost version: ${Boost_VERSION}")
target_include_directories(tests PUBLIC
  "${CMAKE_SOURCE_DIR}/.."
  ${Boost_INCLUDE_DIR}
  )

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
enable_testing()
