#pragma once
#include <pch.hpp>

namespace hcc {
class Lexer {
private:
	uint64_t pos;
	uint64_t line;

public:
	std::string code;

public:
	typedef enum class TokenType {
		NONE,
		END,
		IDENTIFIER,
		INTEGER,
		LPAREN,
		RPAREN,
		LSBRACKET,
		RSBRACKET,
		RETURN,
		SEMICOLON,
		COMMA,
		STRING,
		ADD,
		SUB,
		MUL,
		DIV,
		EQUAL,
	} TokenType;

	typedef struct Token {
		std::variant<int, std::string, char> value;
		TokenType type;
		uint64_t line;
	} Token;

	Lexer(std::string code = "");

	static inline constexpr bool is_letter(char c);
	static inline constexpr bool is_identifier_letter(char c);
	static inline constexpr bool is_digit(char c);

	Token next();
	Token peek(uint32_t count_forward = 1);

private:
	Token identifier();
	Token integer();
	Token string();
};
} // namespace hcc