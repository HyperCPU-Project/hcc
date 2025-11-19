#include "parser/parser.hpp"
#include <gtest/gtest.h>

std::unique_ptr<hcc::AstNode*> parse(std::string code) {
  hcc::Lexer lexer(code);
  hcc::Parser parser(lexer);

  parser.Parse();

  return std::move(parser.root);
}

TEST(HCCTest, ParserMainTest) {
  auto root = parse("int main(){return 0;}");
}
