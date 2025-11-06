cmake_minimum_required(VERSION 3.20)

include("../../cmake/configuration.cmake")

include_directories(".")

file(GLOB HCC_COMMON_SOURCES CONFIGURE_DEPENDS "*.cc")
add_library(hcc_common STATIC ${HCC_COMMON_SOURCES})
target_precompile_headers(hcc_common PRIVATE ../pch.hpp)
target_link_libraries(hcc_common fmt::fmt-header-only)
