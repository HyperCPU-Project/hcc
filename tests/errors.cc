#include <compile.hpp>
#include <gtest/gtest.h>

TEST(HCCTest, UnknownBackendErrorTest) {
  auto result = compile_quick("int main() {return 0;}", "ThisBackendDoesNotExist");

  EXPECT_EQ(result.is_error(), true);
  if (result.is_error()) {
    EXPECT_EQ(result.get_error().value(), "no such backend");
  }
}

TEST(HCCTest, TypesErrorTest) {
  auto result = compile_quick("int main() {vec x; return x;}", "qproc");

  EXPECT_EQ(result.is_error(), true);
  if (result.is_error()) {
    EXPECT_EQ(hcc_compile_error, "unknown type vec");
  }
}

TEST(HCCTest, UndefVarErrorTest1) {
  auto result = compile_quick("int main() {return x;}", "qproc");

  EXPECT_EQ(result.is_error(), true);
  if (result.is_error()) {
    EXPECT_EQ(hcc_compile_error, "undefined variable x");
  }
}

TEST(HCCTest, UndefVarErrorTest2) {
  auto result = compile_quick("int main() {int z; z = y + 1;}", "qproc");

  EXPECT_EQ(result.is_error(), true);
  if (result.is_error()) {
    EXPECT_EQ(hcc_compile_error, "undefined variable y");
  }
}
