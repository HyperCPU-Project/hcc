cmake_minimum_required(VERSION 3.28)
project(hcc LANGUAGES CXX)

set(CMAKE_C_STANDARD 90)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(WITH_TESTS OFF)

find_package(BISON MODULE REQUIRED)
find_package(FLEX MODULE REQUIRED)
find_package(fmt REQUIRED)
find_package(eternal REQUIRED)
find_package(tl-expected REQUIRED)

if(LINUX)
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(HCC_NO_TESTS EQUAL 0 OR NOT DEFINED HCC_NO_TESTS)
  find_package(GTest REQUIRED)
  add_subdirectory(tests)
endif()

add_subdirectory(src)

find_program(CLANG_FORMAT NAMES clang-format REQUIRED)

file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS
        src/**.cpp src/**.hpp tests/**.cpp tests/**.hpp
)

add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i ${CPP_SOURCES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting code with clang-format and cmake-format"
)