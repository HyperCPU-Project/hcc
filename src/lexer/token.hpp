#pragma once
#include <cstd_pch.hpp>

namespace hcc {
  using TokenValue = std::optional<std::variant<long, std::string>>;
  enum class TokenType {
    Number,
    StringLiteral,
    Identifier,

    Lparen,
    Rparen,
    Lsquirly,
    Rsquirly,
    Semicolon,
    Comma,
    Ampersand,
    Add,
    Sub,
    Mul,
    Div,
    Assign,

    /* keywords */
    Return,
    Asm,
    Register,
  };
  struct Token {
    TokenType type;
    TokenValue value;

    Token(TokenType type, TokenValue value);
  };
} // namespace hcc
