#include "compile.hpp"
#include <gtest/gtest.h>

TEST(HCCTest, FunctionQprocCodegenTest) {
  auto result = compileQuick("int main() {return 0;}", "qproc");

  EXPECT_EQ(result.is_success(), true);

  EXPECT_EQ(compileOutput, R"(main:
movi r0 0
pop ip
)");
}
