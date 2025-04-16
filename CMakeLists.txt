cmake_minimum_required(VERSION 3.20)
project(ncc)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cc")

include("cmake/configuration.cmake")

add_executable(ncc ${SOURCES})
target_precompile_headers(ncc PRIVATE src/pch.hpp)
target_include_directories(ncc PRIVATE src)
target_link_libraries(ncc fmt::fmt)
