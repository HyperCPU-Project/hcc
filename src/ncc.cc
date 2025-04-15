#include <ncc.hpp>
#include <util.hpp>

NCC::NCC() {
	lexer.code = "void main() { return; }";

	Lexer::Token token = lexer.next();
	while (token.type != Lexer::TokenType::END) {
		fmt::print("{}\n", (int)token.type);
		token = lexer.next();
	}
}
