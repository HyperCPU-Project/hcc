cmake_minimum_required(VERSION 3.20)

include("../cmake/configuration.cmake")

bison_target(Parser bison/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp)
flex_target(Lexer bison/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
add_flex_bison_dependency(Lexer Parser)

file(GLOB HCC_LIB_SOURCES CONFIGURE_DEPENDS "*.cc" "ast/*.cc" "backend/*/*.cc" "backend/*.cc" "value/*.cc")

add_library(hcc_lib STATIC ${HCC_LIB_SOURCES} ${BISON_Parser_OUTPUTS} ${FLEX_Lexer_OUTPUTS})
target_precompile_headers(hcc_lib PRIVATE pch.hpp)
target_include_directories(hcc_lib PRIVATE "." ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(hcc_lib fmt::fmt)
set_target_properties(hcc_lib PROPERTIES OUTPUT_NAME "hcc")

add_executable(hcc "main/main.cc")
target_precompile_headers(hcc PRIVATE pch.hpp)
target_link_libraries(hcc hcc_lib)
target_include_directories(hcc PRIVATE "." ${CMAKE_CURRENT_BINARY_DIR})
