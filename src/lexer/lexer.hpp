#pragma once
#include <dep_pch.hpp>
#include <lexer/token.hpp>
#include <string>
#include <vector>

namespace hcc {
  class Lexer {
  private:
    std::string input;
    char ch;
    size_t pos;

  public:
    Lexer(std::string _input);

    tl::expected<std::vector<Token>, std::string> Tokenize();

  private:
    bool IsDigit(char c);
    bool IsLetter(char c);
    bool IsIdentifierLetter(char c);

    char Peek();
    void Consume();
  };
} // namespace hcc
