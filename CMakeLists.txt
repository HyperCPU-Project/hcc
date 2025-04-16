cmake_minimum_required(VERSION 3.20)
project(hcc)

include("cmake/configuration.cmake")

bison_target(Parser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
             DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp)
flex_target(Lexer src/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
add_flex_bison_dependency(Lexer Parser)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cc")

add_executable(hcc ${SOURCES} ${BISON_Parser_OUTPUTS} ${FLEX_Lexer_OUTPUTS})

target_precompile_headers(hcc PRIVATE src/pch.hpp)
target_include_directories(hcc PRIVATE src ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(hcc fmt::fmt)