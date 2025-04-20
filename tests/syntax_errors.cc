#include "yy.hpp"
#include <compile.hpp>
#include <gtest/gtest.h>

TEST(HCCTest, LineErrorTest) {
	auto result = compile_quick(R"(int
main
(
){
return
})",
															"qproc");

	EXPECT_EQ(result.is_error(), true);
	if (result.is_error()) {
		// The bison parser expects a semicolon at the next token (which is on the 6th line), that's why we check for an error on line 6
		EXPECT_EQ(hcc_parse_error, "error at line 6: syntax error");
	}
}

TEST(HCCTest, SyntaxErrorTest1) {
	auto result = compile_quick(R"(
int main({
  return 0;
}
)",
															"qproc");

	EXPECT_EQ(result.is_error(), true);
	if (result.is_error()) {
		EXPECT_EQ(hcc_parse_error, "error at line 2: syntax error");
	}
}

TEST(HCCTest, SyntaxErrorTest2) {
	auto result = compile_quick(R"(
int main() return 0;
}
)",
															"qproc");

	EXPECT_EQ(result.is_error(), true);
	if (result.is_error()) {
		EXPECT_EQ(hcc_parse_error, "error at line 2: syntax error");
	}
}

TEST(HCCTest, SyntaxErrorTest3) {
	auto result = compile_quick(R"(
int main(){
return (;
}
)",
															"qproc");

	EXPECT_EQ(result.is_error(), true);
	if (result.is_error()) {
		EXPECT_EQ(hcc_parse_error, "error at line 3: syntax error");
	}
}

TEST(HCCTest, SyntaxErrorTest4) {
	auto result = compile_quick(R"(
int main(){

int 1;
}
)",
															"qproc");

	EXPECT_EQ(result.is_error(), true);
	if (result.is_error()) {
		EXPECT_EQ(hcc_parse_error, "error at line 4: syntax error");
	}
}

TEST(HCCTest, SyntaxErrorTest5) {
	auto result = compile_quick(R"(
int main(){

return;
int x;
x = (;
}
)",
															"qproc");

	EXPECT_EQ(result.is_error(), true);
	if (result.is_error()) {
		EXPECT_EQ(hcc_parse_error, "error at line 6: syntax error");
	}
}

TEST(HCCTest, SyntaxErrorTest6) {
	auto result = compile_quick(R"(
main(){}
)",
															"qproc");

	EXPECT_EQ(result.is_error(), true);
	if (result.is_error()) {
		EXPECT_EQ(hcc_parse_error, "error at line 2: syntax error");
	}
}

TEST(HCCTest, SyntaxErrorTest7) {
	auto result = compile_quick(R"(
int main(){
a b c d e f g
}
)",
															"qproc");

	EXPECT_EQ(result.is_error(), true);
	if (result.is_error()) {
		EXPECT_EQ(hcc_parse_error, "error at line 3: syntax error");
	}
}

TEST(HCCTest, SyntaxErrorTest8) {
	auto result = compile_quick(R"(
int main(){ int x;
x = 2 + 2 2; }
)",
															"qproc");

	EXPECT_EQ(result.is_error(), true);
	if (result.is_error()) {
		EXPECT_EQ(hcc_parse_error, "error at line 3: syntax error");
	}
}

TEST(HCCTest, SyntaxErrorTest9) {
	auto result = compile_quick(R"(
int main(){ int x;

x = 2 + 2 * 2 ); }
)",
															"qproc");

	EXPECT_EQ(result.is_error(), true);
	if (result.is_error()) {
		EXPECT_EQ(hcc_parse_error, "error at line 4: syntax error");
	}
}

TEST(HCCTest, SyntaxErrorTest10) {
	auto result = compile_quick(R"(
asm(123)
)",
															"qproc");

	EXPECT_EQ(result.is_error(), true);
	if (result.is_error()) {
		EXPECT_EQ(hcc_parse_error, "error at line 2: syntax error");
	}
}

TEST(HCCTest, SyntaxErrorTest11) {
	auto result = compile_quick(R"(
int main(){
asm();
}
)",
															"qproc");

	EXPECT_EQ(result.is_error(), true);
	if (result.is_error()) {
		EXPECT_EQ(hcc_parse_error, "error at line 3: syntax error");
	}
}
