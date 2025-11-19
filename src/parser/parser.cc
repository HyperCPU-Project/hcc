#include "parser/parser.hpp"

hcc::Parser::Parser(hcc::Lexer& _lexer) : lexer(_lexer), root(nullptr) {
}

void hcc::Parser::Parse() {
  {
    auto result = lexer.Tokenize();
    if (!result.has_value()) {
      throw std::runtime_error("tokenizer error: " + result.error());
    }

    tokens = result.value();
  }
}
