cmake_minimum_required(VERSION 3.28)
project(hcc LANGUAGES CXX)

set(CMAKE_C_STANDARD 90)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(WITH_TESTS OFF)

include(findBISON)

find_package(BISON CONFIG REQUIRED)
find_package(FLEX REQUIRED)
find_package(fmt REQUIRED)
find_package(eternal REQUIRED)

if(LINUX)
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src)

if(HCC_NO_TESTS EQUAL 0 OR NOT DEFINED HCC_NO_TESTS)
  find_package(GTest REQUIRED)
  add_subdirectory(tests)
endif()

add_subdirectory(src)
