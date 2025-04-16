set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED YES)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(fmt)

if(LINUX)
	add_compile_options("-Wall" "-Wextra" "-Wpedantic")
endif()