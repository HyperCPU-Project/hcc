#include <parser/ast.hpp>

void AstNode::print() {
	fmt::print("(non-inherited node)\n");
}

AstNode::~AstNode() {
	for (AstNode* child : children) {
		delete child;
	}
}

void AstRootNode::print() {
	fmt::printf("(root node)");
}

void AstFuncDef::print() {
	fmt::printf("%s {}\n", name);
}

AstFuncDef* AstFuncDef::create(Parser* parser) {
	AstFuncDef* me = new AstFuncDef();

	Lexer::Token type = parser->currentToken;
	Lexer::Token name = parser->lexer.next();

	if (name.type != Lexer::TokenType::IDENTIFIER) {
		fmt::print("[ncc] [%d] name should be an identifier", name.line);
	}

	me->name = std::get<std::string>(name.value);
	me->return_type = *parser->backend->getTypeFromName(std::get<std::string>(type.value));

	return me;
}