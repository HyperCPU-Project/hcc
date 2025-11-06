cmake_minimum_required(VERSION 3.20)

include("../../cmake/configuration.cmake")

bison_target(Parser bison/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hpp)
flex_target(Lexer bison/lexer.ll ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
add_flex_bison_dependency(Lexer Parser)

file(GLOB HCC_LIB_SOURCES CONFIGURE_DEPENDS "*.cc" "ast/*.cc" "backend/*/*.cc" "backend/*.cc" "value/*.cc" "ir/*.cc" "ir/optimizations/*.cc")

include_directories(".")

add_library(hcc_core STATIC ${HCC_LIB_SOURCES} ${BISON_Parser_OUTPUTS} ${FLEX_Lexer_OUTPUTS})
target_precompile_headers(hcc_core PRIVATE pch.hpp)
target_include_directories(hcc_core PRIVATE ".." "../Common" ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(hcc_core fmt::fmt-header-only)
target_link_libraries(hcc_core hcc_common)