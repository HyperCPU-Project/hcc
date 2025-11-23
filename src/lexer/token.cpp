#include <lexer/token.hpp>
hcc::Token::Token(TokenType _type, TokenValue _value)
    : type(_type), value(_value) {
}
