#include "compile.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("HCpuCodegenTest1") {
  auto result = compileQuick("int main(){ return 0; }", "hypercpu");
  REQUIRE(result.value() == R"(main:
mov x0, 0u0;
ret;
)");
}

TEST_CASE("HCpuCodegenOptimizedTest2") {
  auto result = compileQuick("int main(){ return 2+2*2; }", "hypercpu");
  REQUIRE(result.value() == R"(main:
mov x0, 0u6;
ret;
)");
}
