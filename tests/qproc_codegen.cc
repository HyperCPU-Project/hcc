#include <compile.hpp>
#include <gtest/gtest.h>

TEST(HCCTest, FunctionQprocCodegenTest) {
	auto result = compile_quick("int main() {return 0;}", "qproc");

	EXPECT_EQ(result.is_success(), true);

	EXPECT_EQ(compile_output, R"(main:
push bp
mov bp sp

movi r1 0

mov r0 r1
mov sp bp
pop bp
pop ip

)");
}
