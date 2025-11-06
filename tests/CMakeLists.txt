include("../cmake/configuration.cmake")

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "*.cc")

add_executable(hcctest ${TEST_SOURCES})
target_link_libraries(hcctest gtest gtest_main hcc_core)
target_include_directories(hcctest PRIVATE "." "../src" "../src/Core" "../src/Common")
target_precompile_headers(hcctest PRIVATE pch.hpp)

include(GoogleTest)
gtest_discover_tests(hcctest)
