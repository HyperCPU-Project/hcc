#include <parser/ast.hpp>

void AstNode::print() {
	fmt::print("(non-inherited node)\n");
}

void AstNode::assemble([[maybe_unused]] NCC* ncc) {
	for (AstNode* node : children) {
		node->assemble(ncc);
	}
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
		delete me;
		return nullptr;
	}

	me->name = std::get<std::string>(name.value);
	me->return_type = *parser->backend->getTypeFromName(std::get<std::string>(type.value));

	// Parse function parameters
	Lexer::Token next = parser->lexer.next();
	if (next.type != Lexer::TokenType::LPAREN) {
		fmt::print("[ncc] [{}] expected '(' after function name\n", next.line);
		delete me;
		return nullptr;
	}

	next = parser->lexer.next();
	if (next.type != Lexer::TokenType::RPAREN) {
		fmt::print("[ncc] [{}] expected ')' after function parameters\n", next.line);
		delete me;
		return nullptr;
	}

	// Parse function body
	if (!parser->doBlock(me)) {
		delete me;
		return nullptr;
	}

	return me;
}

void AstFuncDef::assemble(NCC* ncc) {
	ncc->assembly_output += this->name + ":\n";
	ncc->assembly_output += ncc->backend->emit_function_prologue() + "\n";

	AstNode::assemble(ncc);
}

AstReturnNode* AstReturnNode::create(Parser* parser) {
	AstReturnNode* me = new AstReturnNode();

	Lexer::Token next = parser->lexer.next();
	if (next.type != Lexer::TokenType::SEMICOLON) {
		fmt::print("[ncc] [{}] expected semicolon after return\n", next.line);
		delete me;
		return nullptr;
	}

	return me;
}

void AstReturnNode::assemble(NCC* ncc) {
	ncc->assembly_output += ncc->backend->emit_function_epilogue() + "\n";
}

void AstReturnNode::print() {
	fmt::printf("return;\n");
}