add_library(tests-pch INTERFACE cstd_pch.hpp)
target_link_libraries(tests-pch INTERFACE eternal::eternal)

add_executable(hcctest
    compile.cc
		lexer.cc
		hcpu_codegen.cc
		parse_errors.cc
    unknown_backend_errors.cc
)
target_link_libraries(hcctest Catch2::Catch2WithMain hcc_core tests-pch)
target_include_directories(hcctest PRIVATE "." "${PROJECT_SOURCE_DIR}/src")
target_precompile_headers(hcctest PRIVATE cstd_pch.hpp)
