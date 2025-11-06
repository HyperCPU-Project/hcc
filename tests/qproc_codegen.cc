#include "compile.hpp"
#include <gtest/gtest.h>

TEST(HCCTest, FunctionQprocCodegenTest) {
  auto result = compile_quick("int main() {return 0;}", "qproc");

  EXPECT_EQ(result.is_success(), true);

  EXPECT_EQ(compile_output, R"(main:
movi r0 0
pop ip
)");
}
