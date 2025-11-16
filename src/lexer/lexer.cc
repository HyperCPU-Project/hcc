#include <lexer/lexer.hpp>

hcc::Lexer::Lexer(std::string _input) : input(_input), ch(0), pos(0) {
  if (!input.empty()) {
    ch = input[0];
  }
}

tl::expected<std::vector<hcc::Token>, std::string> hcc::Lexer::Tokenize() {
  std::vector<hcc::Token> tokens;

  auto keywordMapping =
      std::map<std::string, TokenType>({{"return", TokenType::Return},
                                        {"int", TokenType::Int},
                                        {"char", TokenType::Char}});
  auto charMapping = std::map<char, TokenType>({{';', TokenType::Semicolon},
                                                {'(', TokenType::Lparen},
                                                {')', TokenType::Rparen},
                                                {'{', TokenType::Lsquirly},
                                                {'}', TokenType::Rsquirly}});

  while (pos < input.size()) {
    if (IsDigit(ch) || ch == '-') {
      if (ch == '-') {
        Consume();
        if (!IsDigit(ch))
          return tl::unexpected("expected a digit after -");
      }

      long num = 0;
      while (IsDigit(ch)) {
        num *= 10;
        num += ch - '0';
        Consume();
      }
      tokens.push_back(Token(TokenType::Number, num));
    } else if (IsLetter(ch)) {
      std::string s;
      while (IsIdentifierLetter(ch)) {
        s.push_back(ch);
        Consume();
      }

      if (keywordMapping.contains(s)) {
        tokens.push_back(Token(keywordMapping[s], {}));
      } else
        tokens.push_back(Token(TokenType::Identifier, s));
    } else if (ch == '"') {
      Consume();

      std::string s;
      while (ch != '"') {
        s.push_back(ch);
        Consume();
      }
      tokens.push_back(Token(TokenType::StringLiteral, s));
      Consume();
    } else if (charMapping.contains(ch)) {
      tokens.push_back(Token(charMapping[ch], {}));
      Consume();
    } else if (ch == ' ')
      Consume();
    else {
      return tl::unexpected(fmt::format("unknown char: {}", ch));
    }
  }

  return tokens;
}

bool hcc::Lexer::IsDigit(char c) {
  return ((c >= '0') && (c <= '9'));
}

bool hcc::Lexer::IsLetter(char c) {
  return (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')));
}

bool hcc::Lexer::IsIdentifierLetter(char c) {
  return IsLetter(c) || IsDigit(c) || c == '_';
}

char hcc::Lexer::Peek() {
  size_t peek_pos = pos + 1;
  return peek_pos >= input.size() ? 0 : input[peek_pos];
}

void hcc::Lexer::Consume() {
  pos++;
  ch = pos < input.size() ? input[pos] : 0;
}
