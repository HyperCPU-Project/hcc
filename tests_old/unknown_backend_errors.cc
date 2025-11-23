#include "compile.hpp"
#include <gtest/gtest.h>

TEST(HCCTest, UnknownBackendTest) {
  auto result = compileQuick("", "abcdefg");
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "no such backend");
}
