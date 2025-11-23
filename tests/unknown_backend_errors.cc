#include "compile.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UnknownBackendTest") {
  auto result = compileQuick("", "abcdefg");
  REQUIRE(!result.has_value());
  REQUIRE(result.error() == "no such backend");
}
