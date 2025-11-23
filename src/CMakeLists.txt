bison_target(Parser parser/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hpp)
#add__bison_dependency(Parser)

add_library(pch INTERFACE)
target_precompile_headers(pch INTERFACE cstd_pch.hpp dep_pch.hpp)
target_link_libraries(pch INTERFACE eternal::eternal)

add_library(hcc_core STATIC
    hcc.cpp
    util.cpp
    ast/ast.cpp
    ast/ast_addrof.cpp
    ast/ast_asm.cpp
    ast/ast_binary_op.cpp
    ast/ast_func_call.cpp
    ast/ast_func_def.cpp
    ast/ast_number.cpp
    ast/ast_return.cpp
    ast/ast_root_node.cpp
    ast/ast_var_assign.cpp
    ast/ast_var_declare.cpp
    ast/ast_var_ref.cpp
    backend/backend.cpp
    backend/hypercpu/hypercpu_backend.cpp
    backend/qproc/qproc_backend.cpp
    ir/ir.cpp
    ir/optimizations/constant_propagation.cpp
    ir/optimizations/dce.cpp
    ir/optimizations/stack_reserve.cpp
    ir/optimizations/stack_setup.cpp
    value/value.cpp
		driver/driver.cpp
		lexer/lexer.cpp
		lexer/token.cpp
    regallocator/register_allocator.cpp
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)
target_include_directories(hcc_core PUBLIC . ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(hcc_core fmt::fmt tl::expected pch)

add_executable(hcc main/main.cpp)
target_link_libraries(hcc PRIVATE hcc_core)
