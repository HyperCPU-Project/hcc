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
	ncc->currentFunction.name = this->name;
	ncc->currentFunction.return_type = return_type;
	ncc->currentFunction.variables.clear();

	ncc->assembly_output += this->name + ":\n";
	ncc->assembly_output += ncc->backend->emit_function_prologue() + "\n";

	AstNode::assemble(ncc);

	ncc->currentFunction.name = "";
	ncc->currentFunction.variables.clear();
}

void AstReturnNode::assemble(NCC* ncc) {
	return_expression->assemble(ncc);
	ncc->assembly_output += ncc->backend->emit_function_epilogue() + "\n";
}

void AstReturnNode::print() {
	fmt::printf("return;\n");
}

AstBinaryOpNode* AstBinaryOpNode::create([[maybe_unused]] Parser* parser, Operation op, AstNode* left, AstNode* right) {
	AstBinaryOpNode* me = new AstBinaryOpNode();
	me->op = op;
	me->left = left;
	me->right = right;
	return me;
}

void AstBinaryOpNode::print() {
	fmt::printf("(");
	left->print();
	fmt::printf(" %s ", op == Operation::ADD ? "+" : "*");
	right->print();
	fmt::printf(")");
}

void AstBinaryOpNode::assemble(NCC* ncc) {
	int32_t LHS, RHS;
	LHS = 0;
	RHS = 1;

	left->assemble(ncc);
	right->assemble(ncc);

#ifdef BACKEND_QPROC
	if (QprocBackend* backend = dynamic_cast<QprocBackend*>(ncc->backend)) {
		RHS = backend->previous_reg_indexes.top();
		backend->previous_reg_indexes.pop();
		LHS = backend->previous_reg_indexes.top();
		backend->previous_reg_indexes.pop();
	}
#endif

	if (LHS != -1 && RHS != -1) {
		switch (op) {
		case Operation::ADD: {
			ncc->assembly_output += ncc->backend->emit_add(12, LHS, RHS);
		} break;
		case Operation::MUL: {
			ncc->assembly_output += ncc->backend->emit_mul(12, LHS, RHS);
		} break;
		}
	}
}

AstNumberNode* AstNumberNode::create(Parser* parser) {
	AstNumberNode* me = new AstNumberNode();
	me->value = std::get<int>(parser->currentToken.value);
	return me;
}

void AstNumberNode::print() {
	fmt::printf("%d", value);
}

void AstNumberNode::assemble(NCC* ncc) {
	fmt::print("ok\n");
	ncc->assembly_output += ncc->backend->emit_mov_const(this->value);
}

AstReturnNode* AstReturnNode::create(Parser* parser) {
	AstReturnNode* me = new AstReturnNode();

	parser->currentToken = parser->lexer.next(); // consume 'return'
	me->return_expression = parser->parseExpression();
	if (!me->return_expression) {
		delete me;
		return nullptr;
	}

	/*Lexer::Token next = parser->lexer.next();
	if (next.type != Lexer::TokenType::SEMICOLON) {
		fmt::print("[ncc] [{}] expected semicolon after return {}\n", next.line, (int)next.type);
		// delete me;
		// return nullptr;
	}*/

	return me;
}
