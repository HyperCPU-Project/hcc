cmake_minimum_required(VERSION 3.20)

include("../../cmake/configuration.cmake")

add_executable(hcc "main.cc")
target_precompile_headers(hcc PRIVATE pch.hpp)
target_link_libraries(hcc hcc_core)
target_include_directories(hcc PRIVATE "." ".." "../Core" "../Common" ${CMAKE_CURRENT_BINARY_DIR})
