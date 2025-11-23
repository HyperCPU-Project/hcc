#pragma once
#include "ast/ast.hpp"
#include "lexer/lexer.hpp"
#include <memory>

namespace hcc {
  class Driver {
  public:
    explicit Driver();
    ~Driver();

    void set_root(std::unique_ptr<AstNode> node);

    std::unique_ptr<AstNode> root;
    std::vector<Token> tokens;
    std::size_t token_i;
    std::optional<std::string> error;

    friend class Parser;
  };
} // namespace hcc
