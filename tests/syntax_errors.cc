#include "compile.hpp"
#include <gtest/gtest.h>

TEST(HCCTest, LineErrorTest) {
  auto result = compileQuick(R"(int
main
(
){
return
})",
                             "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "error at line 6: syntax error");
}

TEST(HCCTest, SyntaxErrorTest1) {
  auto result = compileQuick(R"(
int main({
  return 0;
}
)",
                             "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "error at line 2: syntax error");
}

TEST(HCCTest, SyntaxErrorTest2) {
  auto result = compileQuick(R"(
int main() return 0;
}
)",
                             "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "error at line 2: syntax error");
}

TEST(HCCTest, SyntaxErrorTest3) {
  auto result = compileQuick(R"(
int main(){
return (;
}
)",
                             "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "error at line 3: syntax error");
}

TEST(HCCTest, SyntaxErrorTest4) {
  auto result = compileQuick(R"(
int main(){

int 1;
}
)",
                             "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "error at line 4: syntax error");
}

TEST(HCCTest, SyntaxErrorTest5) {
  auto result = compileQuick(R"(
int main(){

return;
int x;
x = (;
}
)",
                             "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "error at line 6: syntax error");
}

TEST(HCCTest, SyntaxErrorTest6) {
  auto result = compileQuick(R"(
main(){}
)",
                             "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "error at line 2: syntax error");
}

TEST(HCCTest, SyntaxErrorTest7) {
  auto result = compileQuick(R"(
int main(){
a b c d e f g
}
)",
                             "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "error at line 3: syntax error");
}

TEST(HCCTest, SyntaxErrorTest8) {
  auto result = compileQuick(R"(
int main(){ int x;
x = 2 + 2 2; }
)",
                             "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "error at line 3: syntax error");
}

TEST(HCCTest, SyntaxErrorTest9) {
  auto result = compileQuick(R"(
int main(){ int x;

x = 2 + 2 * 2 ); }
)",
                             "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "error at line 4: syntax error");
}

TEST(HCCTest, SyntaxErrorTest10) {
  auto result = compileQuick(R"(
asm(123)
)",
                             "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "error at line 2: syntax error");
}

TEST(HCCTest, SyntaxErrorTest11) {
  auto result = compileQuick(R"(
int main(){
asm();
}
)",
                             "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "error at line 3: syntax error");
}
