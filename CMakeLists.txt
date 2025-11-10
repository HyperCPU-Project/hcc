cmake_minimum_required(VERSION 3.20)
project(hcc)

include("cmake/configuration.cmake")

option(WITH_TESTS "" OFF)
add_subdirectory("foreign/eternal")

include_directories("foreign/eternal/include") # prolly a bad thing to do, fix this someday

set(EXPECTED_BUILD_TESTS OFF)
add_subdirectory("foreign/expected")
link_libraries(expected)

if(HCC_NO_TESTS EQUAL 0 OR NOT DEFINED HCC_NO_TESTS)
  add_subdirectory("foreign/googletest")
  add_subdirectory("tests/")
endif()

add_subdirectory("src/")
