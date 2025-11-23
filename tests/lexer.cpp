#include "lexer/lexer.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("a", "[single-file]") {
  REQUIRE(1 == 1);
}

template <typename Tv>
static inline void __impl_assert_token_val(hcc::Token token, hcc::TokenType type,
                                           Tv val) {
  REQUIRE(token.value.has_value());
  REQUIRE(std::holds_alternative<Tv>(token.value.value()));
}

static inline void assert_token(hcc::Token token, hcc::TokenType type) {
  REQUIRE(token.type == type);
}

static void assert_token(hcc::Token token, hcc::TokenType type, std::string v) {
  __impl_assert_token_val(token, type, v);
}

static void assert_token(hcc::Token token, hcc::TokenType type, long v) {
  __impl_assert_token_val(token, type, v);
}

#define STRINGIFY(x) #x

#define EMIT_TOKEN_TEST(name, type, code, i)       \
  TEST_CASE(STRINGIFY(Lexer##name##Test)) {        \
    auto tokens = tokenize(code);                  \
    assert_token(tokens[i], hcc::TokenType::type); \
  }

#define EMIT_TOKEN_TEST_VALUE(name, type, value, code, i) \
  TEST_CASE(STRINGIFY(Lexer##name##Test)) {               \
    auto tokens = tokenize(code);                         \
    assert_token(tokens[i], hcc::TokenType::type, value); \
  }

static std::vector<hcc::Token> tokenize(std::string s) {
  auto lexer = hcc::Lexer(s);
  auto result = lexer.Tokenize();
  if (!result.has_value()) {
    throw std::runtime_error(
        fmt::format("tokenize failed: {}", result.error()));
  }
  return result.value();
}

TEST_CASE("LexerMainTest") {
  auto tokens = tokenize("int main(){return 0;}");

  assert_token(tokens[0], hcc::TokenType::Identifier, "int");
  assert_token(tokens[1], hcc::TokenType::Identifier, "main");
  assert_token(tokens[2], hcc::TokenType::Lparen);
  assert_token(tokens[3], hcc::TokenType::Rparen);
  assert_token(tokens[4], hcc::TokenType::Lsquirly);
  assert_token(tokens[5], hcc::TokenType::Return);
  assert_token(tokens[6], hcc::TokenType::Number, 0);
  assert_token(tokens[7], hcc::TokenType::Semicolon);
  assert_token(tokens[8], hcc::TokenType::Rsquirly);
}

EMIT_TOKEN_TEST_VALUE(Int, Identifier, "int", "int main {int", 3);
EMIT_TOKEN_TEST_VALUE(Char, Identifier, "char", "int main {char", 3);
EMIT_TOKEN_TEST(Return, Return, "int main {return", 3);
EMIT_TOKEN_TEST_VALUE(Num, Number, 12345, "int main {12345; ", 3);
EMIT_TOKEN_TEST_VALUE(NegNum, Number, -12345, "int main {-12345; ", 3);
EMIT_TOKEN_TEST_VALUE(String, StringLiteral, "Hello, World",
                      "int main {\"Hello, World\"; ", 3);
EMIT_TOKEN_TEST_VALUE(Identifier, Identifier, "hello123__1",
                      "int main {hello123__1; ", 3);
EMIT_TOKEN_TEST(Lparen, Lparen, "int main {(; ", 3);
EMIT_TOKEN_TEST(Rparen, Rparen, "int main {); ", 3);
EMIT_TOKEN_TEST(Lsquirly, Lsquirly, "int main {{; ", 3);
EMIT_TOKEN_TEST(Rsquirly, Rsquirly, "int main {}; ", 3);
EMIT_TOKEN_TEST(Semicolon, Semicolon, "int main {;; ", 3);
