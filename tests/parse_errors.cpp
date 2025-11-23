#include "compile.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ParseErrorTest1") {
  auto result = compileQuick("int main({ return 0; }", "hypercpu");
  REQUIRE(result.error() == "parse error: syntax error, unexpected LSQUIRLY, expecting RPAREN");
}

TEST_CASE("ParseErrorTest2") {
  auto result = compileQuick("int ({ return 0; }", "hypercpu");
  REQUIRE(result.error() == "parse error: syntax error, unexpected LPAREN, expecting IDENTIFIER");
}

TEST_CASE("ParseErrorTest3") {
  auto result = compileQuick("int main(){ return +; }", "hypercpu");
  REQUIRE(result.error() == "parse error: syntax error, unexpected PLUS");
}

TEST_CASE("ParseErrorTest4") {
  auto result = compileQuick("int main(){ int 1; }", "hypercpu");
  REQUIRE(result.error() == "parse error: syntax error, unexpected NUMBER, expecting IDENTIFIER or LPAREN or ASSIGN");
}
