add_library(tests-pch INTERFACE cstd_pch.hpp)
target_link_libraries(tests-pch INTERFACE eternal::eternal)

add_executable(hcctest
    compile.cc
		lexer.cc
    main.cc
)
target_link_libraries(hcctest gtest::gtest hcc_core tests-pch)
target_include_directories(hcctest PRIVATE "." "${PROJECT_SOURCE_DIR}/src")
target_precompile_headers(hcctest PRIVATE cstd_pch.hpp)
