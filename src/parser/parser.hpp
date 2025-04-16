#pragma once
#include <lexer/lexer.hpp>
#include <parser/ast.hpp>
#include <pch.hpp>

namespace hcc {
class AstRootNode;
class AstNode;
class Backend;

class Parser {
public:
	Backend* backend;
	Lexer& lexer;
	Lexer::Token currentToken;

	AstRootNode* root_node;

	Parser(Lexer& lexer, Backend* backend);
	~Parser();

	bool parse();
	bool doBlock(AstNode* parent);

	AstNode* parseExpression();
	AstNode* parseTerm();
	AstNode* parseFactor();
	AstNode* parseNumber();
};
} // namespace hcc