#include "compile.hpp"
#include <gtest/gtest.h>

TEST(HCCTest, UnknownBackendErrorTest) {
  auto result = compileQuick("int main() {return 0;}", "ThisBackendDoesNotExist");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "no such backend");
}

TEST(HCCTest, TypesErrorTest) {
  auto result = compileQuick("int main() {vec x; return x;}", "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "unknown type vec");
}

TEST(HCCTest, UndefVarErrorTest1) {
  auto result = compileQuick("int main() {return x;}", "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "undefined variable x");
}

TEST(HCCTest, UndefVarErrorTest2) {
  auto result = compileQuick("int main() {int z; z = y + 1;}", "qproc");

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "undefined variable y");
}
