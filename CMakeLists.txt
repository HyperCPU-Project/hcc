cmake_minimum_required(VERSION 3.20)
project(hcc)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cc")

include("cmake/configuration.cmake")

BISON_TARGET(Parser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp)
FLEX_TARGET(Lexer src/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

add_executable(hcc ${SOURCES} ${BISON_Parser_OUTPUTS} ${FLEX_Lexer_OUTPUTS})
target_precompile_headers(hcc PRIVATE src/pch.hpp)
target_include_directories(hcc PRIVATE src)
target_link_libraries(hcc fmt::fmt)
