cmake_minimum_required(VERSION 3.20)

include("../cmake/configuration.cmake")

bison_target(Parser bison/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hpp)
flex_target(Lexer bison/lexer.ll ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
add_flex_bison_dependency(Lexer Parser)

file(GLOB HCC_LIB_SOURCES CONFIGURE_DEPENDS "*.cc" "ast/*.cc" "backend/*/*.cc" "backend/*.cc" "value/*.cc" "ir/*.cc" "ir/optimizations/*.cc")

add_library(hcc_core STATIC ${HCC_LIB_SOURCES} ${BISON_Parser_OUTPUTS} ${FLEX_Lexer_OUTPUTS})
target_link_libraries(hcc_core fmt::fmt-header-only eternal)
target_precompile_headers(hcc_core PRIVATE pch.hpp)
target_include_directories(hcc_core PRIVATE "." ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(hcc_core PROPERTIES OUTPUT_NAME "hcc")

add_executable(hcc "main/main.cc")
target_link_libraries(hcc hcc_core eternal)
target_precompile_headers(hcc PRIVATE pch.hpp)
target_include_directories(hcc PRIVATE "." ${CMAKE_CURRENT_BINARY_DIR})
