#include "compile.hpp"
#include <gtest/gtest.h>

TEST(HCCTest, UnknownBackendErrorTest) {
  auto result = compileQuick("int main() {return 0;}", "ThisBackendDoesNotExist");

  EXPECT_EQ(result.IsError(), true);
  if (result.IsError()) {
    EXPECT_EQ(result.GetError().value(), "no such backend");
  }
}

TEST(HCCTest, TypesErrorTest) {
  auto result = compileQuick("int main() {vec x; return x;}", "qproc");

  EXPECT_EQ(result.IsError(), true);
  if (result.IsError()) {
    EXPECT_EQ(hcc_compile_error, "unknown type vec");
  }
}

TEST(HCCTest, UndefVarErrorTest1) {
  auto result = compileQuick("int main() {return x;}", "qproc");

  EXPECT_EQ(result.IsError(), true);
  if (result.IsError()) {
    EXPECT_EQ(hcc_compile_error, "undefined variable x");
  }
}

TEST(HCCTest, UndefVarErrorTest2) {
  auto result = compileQuick("int main() {int z; z = y + 1;}", "qproc");

  EXPECT_EQ(result.IsError(), true);
  if (result.IsError()) {
    EXPECT_EQ(hcc_compile_error, "undefined variable y");
  }
}
