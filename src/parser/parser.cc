#include <backend/backend.hpp>
#include <parser/ast.hpp>
#include <parser/parser.hpp>

using namespace hcc;

hcc::Parser::Parser(Lexer& lexer, Backend* backend) : backend(backend), lexer(lexer), root_node(nullptr) {
	root_node = new AstRootNode();
}

hcc::Parser::~Parser() {
	if (root_node)
		delete root_node;
}

bool hcc::Parser::parse() {
	Lexer::Token token = lexer.next();
	while (token.type != Lexer::TokenType::END) {
		currentToken = token;
		switch (token.type) {
		case Lexer::TokenType::IDENTIFIER: {
			if (!backend->getTypeFromName(std::get<std::string>(token.value))) {
				fmt::print("[hcc] [{}] invalid type\n", token.line);
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

bool hcc::Parser::doBlock(AstNode* parent) {
	Lexer::Token token = lexer.next();
	if (token.type != Lexer::TokenType::LSBRACKET) {
		fmt::print("[hcc] [{}] expected '{{' at start of block\n", token.line);
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

AstNode* hcc::Parser::parseNumber() {
	if (currentToken.type != Lexer::TokenType::INTEGER) {
		fmt::print("[hcc] [{}] expected number {}\n", currentToken.line, (int)currentToken.type);
		return nullptr;
	}

	AstNumberNode* node = AstNumberNode::create(this);
	currentToken = lexer.next(); // consume the number
	return node;
}

AstNode* hcc::Parser::parseFactor() {
	if (currentToken.type == Lexer::TokenType::LPAREN) {
		currentToken = lexer.next(); // consume '('
		AstNode* expr = parseExpression();
		if (!expr)
			return nullptr;

		if (currentToken.type != Lexer::TokenType::RPAREN) {
			fmt::print("[hcc] [{}] expected ')'\n", currentToken.line);
			delete expr;
			return nullptr;
		}
		currentToken = lexer.next(); // consume ')'
		return expr;
	}

	return parseNumber();
}

AstNode* hcc::Parser::parseTerm() {
	AstNode* left = parseFactor();
	if (!left)
		return nullptr;

	Lexer::Token op = currentToken;
	while (op.type == Lexer::TokenType::MUL || op.type == Lexer::TokenType::DIV) {
		currentToken = lexer.next(); // consume
		AstNode* right = parseFactor();
		if (!right) {
			delete left;
			return nullptr;
		}

		switch (op.type) {
		case Lexer::TokenType::MUL:
			left = AstBinaryOpNode::create(this, AstBinaryOpNode::Operation::MUL, left, right);
			break;
		case Lexer::TokenType::DIV:
			left = AstBinaryOpNode::create(this, AstBinaryOpNode::Operation::DIV, left, right);
			break;
		default:
			break;
		}
		op = currentToken;
	}

	return left;
}

AstNode* hcc::Parser::parseExpression() {
	AstNode* left = parseTerm();
	if (!left)
		return nullptr;

	while (currentToken.type == Lexer::TokenType::ADD || currentToken.type == Lexer::TokenType::SUB) {
		currentToken = lexer.next(); // consume '+'
		AstNode* right = parseTerm();
		if (!right) {
			delete left;
			return nullptr;
		}

		switch (currentToken.type) {
		case Lexer::TokenType::ADD:
			left = AstBinaryOpNode::create(this, AstBinaryOpNode::Operation::ADD, left, right);
			break;
		case Lexer::TokenType::SUB:
			left = AstBinaryOpNode::create(this, AstBinaryOpNode::Operation::SUB, left, right);
			break;
		default:
			break;
		}
	}

	return left;
}
