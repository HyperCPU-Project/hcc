#pragma once
#include <ast/ast.hpp>
#include <pch.hpp>

namespace hcc {
  struct Parser;
}

typedef struct yy_buffer_state* YY_BUFFER_STATE;

YY_BUFFER_STATE yy_scan_string(const char* str, void*);
void yy_delete_buffer(YY_BUFFER_STATE buffer, void*);
int yyparse(void*, hcc::Parser*);

extern FILE* yyin;

extern std::string hcc_parse_error;
