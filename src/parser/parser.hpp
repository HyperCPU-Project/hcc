#pragma once

#include "ast/ast.hpp"
#include "cstd_pch.hpp"
#include "lexer/lexer.hpp"

namespace hcc {
  class Parser {
  protected:
    Lexer& lexer;
    std::vector<Token> tokens;

  public:
    std::unique_ptr<AstNode*> root;

  public:
    Parser(Lexer& lexer);

  protected:
    void ParseFunction();

  public:
    void Parse();
  };
} // namespace hcc
