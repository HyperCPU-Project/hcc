cmake_minimum_required(VERSION 3.20)
project(hcc)

include("cmake/configuration.cmake")

add_subdirectory("src/")

if(HCC_NO_TESTS EQUAL 0)
  add_subdirectory("dist/googletest")
  add_subdirectory("tests/")
endif()
