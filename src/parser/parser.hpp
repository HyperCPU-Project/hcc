#pragma once
#include <backend/backend.hpp>
#include <lexer/lexer.hpp>
#include <parser/ast.hpp>
#include <pch.hpp>

class AstRootNode;
class AstNode;

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
};