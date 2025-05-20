#include <compile.hpp>
#include <gtest/gtest.h>

TEST(HCCTest, FunctionQprocCodegenTest) {
  auto result = compile_quick("int main() {return 0;}", "qproc");

  EXPECT_EQ(result.is_success(), true);

  EXPECT_EQ(compile_output, R"(; emit_function_prologue
main:
push bp
mov bp sp
; emit_mov_const
movi r1 0
; emit_move
mov r0 r1
; emit_function_epilogue
mov sp bp
pop bp
pop ip
)");
}
