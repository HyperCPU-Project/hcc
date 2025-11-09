bison_target(Parser bison/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hpp)
flex_target(Lexer bison/lexer.ll ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
add_flex_bison_dependency(Lexer Parser)

add_library(hcc_core STATIC
    hcc.cc
    util.cc
    ast/ast.cc
    ast/ast_addrof.cc
    ast/ast_asm.cc
    ast/ast_binary_op.cc
    ast/ast_func_call.cc
    ast/ast_func_def.cc
    ast/ast_number.cc
    ast/ast_return.cc
    ast/ast_root_node.cc
    ast/ast_var_assign.cc
    ast/ast_var_declare.cc
    ast/ast_var_ref.cc
    backend/backend.cc
    backend/hypercpu/hypercpu_backend.cc
    backend/qproc/qproc_backend.cc
    ir/ir.cc
    ir/optimizations/constant_propagation.cc
    ir/optimizations/dce.cc
    ir/optimizations/stack_reserve.cc
    ir/optimizations/stack_setup.cc
    value/value.cc
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)
target_precompile_headers(hcc_core PRIVATE pch.hpp)
target_include_directories(hcc_core PRIVATE "." ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(hcc_core fmt::fmt)
set_target_properties(hcc_core PROPERTIES OUTPUT_NAME "hcc")

add_executable(hcc main/main.cc)
target_precompile_headers(hcc PRIVATE pch.hpp)
target_include_directories(hcc PRIVATE "." ${CMAKE_CURRENT_BINARY_DIR})
