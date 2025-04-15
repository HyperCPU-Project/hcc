#include <parser/ast.hpp>
#include <parser/parser.hpp>

Parser::Parser(Lexer& lexer, Backend* backend) : backend(backend), lexer(lexer), root_node(nullptr) {
	root_node = new AstRootNode();
}

Parser::~Parser() {
	if (root_node)
		delete root_node;
}

bool Parser::parse() {
	Lexer::Token token = lexer.next();
	while (token.type != Lexer::TokenType::END) {
		currentToken = token;
		switch (token.type) {
		case Lexer::TokenType::IDENTIFIER: {
			if (!backend->getTypeFromName(std::get<std::string>(token.value))) {
				fmt::print("[ncc] [{}] invalid type\n", token.line);
				return false;
			}

			AstFuncDef* funcdef = AstFuncDef::create(this);
			if (!funcdef)
				return false;
			root_node->children.push_back(funcdef);
		} break;
		default:
			break;
		}

		token = lexer.next();
	}

	return true;
}

bool Parser::doBlock(AstNode* parent) {
	Lexer::Token token = lexer.next();
	if (token.type != Lexer::TokenType::LSBRACKET) {
		fmt::print("[ncc] [{}] expected '{{' at start of block\n", token.line);
		return false;
	}

	token = lexer.next();
	while (token.type != Lexer::TokenType::RSBRACKET) {
		currentToken = token;
		switch (token.type) {
		case Lexer::TokenType::RETURN: {
			AstReturnNode* ret = AstReturnNode::create(this);
			if (ret == nullptr)
				return false;
			parent->children.push_back(ret);
		} break;
		default:
			break;
		}

		token = lexer.next();
	}

	return true;
}