#include <backend/backend.hpp>
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

AstNode* Parser::parseNumber() {
	if (currentToken.type != Lexer::TokenType::INTEGER) {
		fmt::print("[ncc] [{}] expected number {}\n", currentToken.line, (int)currentToken.type);
		return nullptr;
	}

	AstNumberNode* node = AstNumberNode::create(this);
	currentToken = lexer.next(); // consume the number
	return node;
}

AstNode* Parser::parseFactor() {
	if (currentToken.type == Lexer::TokenType::LPAREN) {
		currentToken = lexer.next(); // consume '('
		AstNode* expr = parseExpression();
		if (!expr)
			return nullptr;

		if (currentToken.type != Lexer::TokenType::RPAREN) {
			fmt::print("[ncc] [{}] expected ')'\n", currentToken.line);
			delete expr;
			return nullptr;
		}
		currentToken = lexer.next(); // consume ')'
		return expr;
	}

	return parseNumber();
}

AstNode* Parser::parseTerm() {
	AstNode* left = parseFactor();
	if (!left)
		return nullptr;

	while (currentToken.type == Lexer::TokenType::MUL) {
		currentToken = lexer.next(); // consume '*'
		AstNode* right = parseFactor();
		if (!right) {
			delete left;
			return nullptr;
		}
		left = AstBinaryOpNode::create(this, AstBinaryOpNode::Operation::MUL, left, right);
	}

	return left;
}

AstNode* Parser::parseExpression() {
	AstNode* left = parseTerm();
	if (!left)
		return nullptr;

	while (currentToken.type == Lexer::TokenType::ADD) {
		currentToken = lexer.next(); // consume '+'
		AstNode* right = parseTerm();
		if (!right) {
			delete left;
			return nullptr;
		}
		left = AstBinaryOpNode::create(this, AstBinaryOpNode::Operation::ADD, left, right);
	}

	return left;
}
