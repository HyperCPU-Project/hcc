#include <lexer/lexer.hpp>

hcc::Lexer::Lexer(std::string code) : code(code) {
	pos = 0;
	line = 0;
}

inline constexpr bool hcc::Lexer::is_letter(char c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

inline constexpr bool hcc::Lexer::is_identifier_letter(char c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || is_digit(c));
}

inline constexpr bool hcc::Lexer::is_digit(char c) {
	return (c >= '0' && c <= '9');
}

hcc::Lexer::Token hcc::Lexer::integer() {
	Token tok;
	bool is_hex;

	tok.type = TokenType::INTEGER;
	tok.line = line;
	tok.value = 0;

	is_hex = false;

	while (pos < code.length()) {
		char& c = code.at(pos);
		if (!is_digit(c) && !is_hex) {
			if (c == 'x') {
				is_hex = true;
				tok.value = 0;
				pos++;
				continue;
			} else {
				break;
			}
		}

		if (is_hex) {
			bool invalid;

			invalid = false;
			std::get<int>(tok.value) *= 16;

			if (is_digit(c)) {
				std::get<int>(tok.value) += c - '0';
			} else {
				switch (c) {
				case 'a':
					std::get<int>(tok.value) += 0xa;
					break;
				case 'b':
					std::get<int>(tok.value) += 0xb;
					break;
				case 'c':
					std::get<int>(tok.value) += 0xc;
					break;
				case 'd':
					std::get<int>(tok.value) += 0xd;
					break;
				case 'e':
					std::get<int>(tok.value) += 0xe;
					break;
				case 'f':
					std::get<int>(tok.value) += 0xf;
					break;
				default:
					invalid = true;
					break;
				}
			}
			if (invalid) {
				std::get<int>(tok.value) /= 16;
				break;
			}
		} else {
			std::get<int>(tok.value) *= 10;

			std::get<int>(tok.value) += c - '0';
		}
		pos++;
	}

	return tok;
}

hcc::Lexer::Token hcc::Lexer::identifier() {
	Token tok;

	tok.type = TokenType::IDENTIFIER;
	tok.line = line;
	tok.value = "";

	while (pos < code.length()) {
		char& c = code.at(pos);

		if (!is_identifier_letter(c) && !is_digit(c)) {
			break;
		}

		std::get<std::string>(tok.value).push_back(c);

		pos++;
	}

	return tok;
}

hcc::Lexer::Token hcc::Lexer::string() {
	Token tok;
	bool definition;

	tok.type = TokenType::STRING;
	tok.line = line;
	tok.value = "";

	definition = false;
	while (pos < code.length()) {
		char& c = code.at(pos);

		if (c == '"') {
			if (definition) {
				pos++;
				break;
			}
			definition = true;
			pos++;
			continue;
		}

		if (definition) {
			std::get<std::string>(tok.value).push_back(c);
		}

		pos++;
	}

	return tok;
}

hcc::Lexer::Token hcc::Lexer::next() {
	while (pos < code.length()) {
		Token token{0, TokenType::NONE, line};
		char& c = code.at(pos);

		if (is_letter(c)) {
			token = identifier();
			std::string value = std::get<std::string>(token.value);
			if (value == "return") {
				token.type = TokenType::RETURN;
			}
		} else if (is_digit(c)) {
			token = integer();
		} else if (c == '"') {
			token = string();
		} else {
			switch (c) {
			case '(':
				token.type = TokenType::LPAREN;
				pos++;
				break;
			case ')':
				token.type = TokenType::RPAREN;
				pos++;
				break;
			case '}':
				token.type = TokenType::RSBRACKET;
				pos++;
				break;
			case '{':
				token.type = TokenType::LSBRACKET;
				pos++;
				break;
			case ',':
				token.type = TokenType::COMMA;
				pos++;
				break;
			case ';':
				token.type = TokenType::SEMICOLON;
				pos++;
				break;
			case '+':
				token.type = TokenType::ADD;
				pos++;
				break;
			case '-':
				token.type = TokenType::SUB;
				pos++;
				break;
			case '*':
				token.type = TokenType::MUL;
				pos++;
				break;
			case '/':
				token.type = TokenType::DIV;
				pos++;
				break;
			}
		}

		if (token.type != TokenType::NONE) {
			return token;
		}

		pos++;
	}
	return Token{0, TokenType::END, line};
}

hcc::Lexer::Token hcc::Lexer::peek(uint32_t count_forward) {
	uint64_t old_pos = pos;

	Token token{0, TokenType::END, line};
	for (uint32_t i = 0; i < count_forward; i++) {
		token = next();
	}

	pos = old_pos;
	return token;
}
