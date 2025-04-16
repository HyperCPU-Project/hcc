cmake_minimum_required(VERSION 3.20)
project(hcc)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cc")

include("cmake/configuration.cmake")

add_executable(hcc ${SOURCES})
target_precompile_headers(hcc PRIVATE src/pch.hpp)
target_include_directories(hcc PRIVATE src)
target_link_libraries(hcc fmt::fmt)
