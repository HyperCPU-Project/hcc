#include "compile.hpp"
#include <gtest/gtest.h>

TEST(HCCTest, FunctionQprocCodegenTest) {
  auto result = compileQuick("int main() {return 0;}", "qproc");

  ASSERT_TRUE(result.has_value());

  EXPECT_EQ(compile_output, R"(main:
movi r0 0
pop ip
)");
}
