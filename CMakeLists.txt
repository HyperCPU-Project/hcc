cmake_minimum_required(VERSION 3.20)
project(ncc)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED YES)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cc")

if(LINUX)
	add_compile_options("-Wall" "-Wextra" "-Wpedantic")
endif()

find_package(fmt)

add_executable(ncc ${SOURCES})
target_precompile_headers(ncc PRIVATE src/pch.hpp)
target_include_directories(ncc PRIVATE src)
target_link_libraries(ncc fmt::fmt)
