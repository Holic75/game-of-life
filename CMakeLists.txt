cmake_minimum_required (VERSION 3.8)
project ("GAME_OF_LIFE")
option(BUILD_TESTS "Build test programs" OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_subdirectory("src")

file(COPY examples DESTINATION ${PROJECT_BINARY_DIR})
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory("test")
endif(BUILD_TESTS)
