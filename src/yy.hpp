#pragma once
#include <ast/ast.hpp>
#include <pch.hpp>

typedef struct yy_buffer_state* YY_BUFFER_STATE;

YY_BUFFER_STATE yy_scan_string(const char* str);
void yy_delete_buffer(YY_BUFFER_STATE buffer);
int yyparse();

extern FILE* yyin;

extern hcc::AstRootNode* root;
extern std::string hcc_parse_error;

extern int line_num;
