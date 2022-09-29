cmake_minimum_required(VERSION 3.22)
project(tests)

Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.1 # or a later release
  )
FetchContent_MakeAvailable(Catch2)

add_executable( tests UtcDateTimeTests.cpp)
set(BOOST_INCLUDE_DIR "/mnt/c/Users/ppato/source/boost.1.80.0/lib/native/include/")
message("BOOST_INCLUDE_DIR: ${BOOST_INCLUDE_DIR}")
target_include_directories(tests PUBLIC
  "${CMAKE_SOURCE_DIR}/.."
  ${BOOST_INCLUDE_DIR})

# ${BOOST_INCLUDE_DIRS}

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
enable_testing()
