cmake_minimum_required(VERSION 3.20)
project(hcc)

include("cmake/configuration.cmake")

add_subdirectory("dist/googletest")
add_subdirectory("src/")
add_subdirectory("tests/")
