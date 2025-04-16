#include <backend/backend.hpp>
#include <parser/ast.hpp>
#include <value.hpp>

static std::stack<hcc::Value> values;

void hcc::AstNode::print() {
	for (AstNode* node : children) {
		node->print();
	}
}

void hcc::AstNode::assemble([[maybe_unused]] HCC* hcc) {
	for (AstNode* node : children) {
		node->assemble(hcc);
	}
}

hcc::AstNode::~AstNode() {
	for (AstNode* child : children) {
		delete child;
	}
}

void hcc::AstRootNode::print() {
	fmt::printf("(root node)\n");
	AstNode::print();
}

void hcc::AstFuncDef::print() {
	fmt::printf("%s {\n", name);
	AstNode::print();
	fmt::printf("}\n", name);
}

hcc::AstFuncDef* hcc::AstFuncDef::create(Parser* parser) {
	AstFuncDef* me = new AstFuncDef();

	Lexer::Token type = parser->currentToken;
	Lexer::Token name = parser->lexer.next();

	if (name.type != Lexer::TokenType::IDENTIFIER) {
		fmt::print("[hcc] [%d] name should be an identifier", name.line);
		delete me;
		return nullptr;
	}

	me->name = std::get<std::string>(name.value);
	me->return_type = *parser->backend->getTypeFromName(std::get<std::string>(type.value));

	// Parse function parameters
	Lexer::Token next = parser->lexer.next();
	if (next.type != Lexer::TokenType::LPAREN) {
		fmt::print("[hcc] [{}] expected '(' after function name\n", next.line);
		delete me;
		return nullptr;
	}

	next = parser->lexer.next();
	if (next.type != Lexer::TokenType::RPAREN) {
		fmt::print("[hcc] [{}] expected ')' after function parameters\n", next.line);
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

void hcc::AstFuncDef::assemble(HCC* hcc) {
	hcc->currentFunction.name = this->name;
	hcc->currentFunction.return_type = return_type;
	hcc->currentFunction.variables.clear();

	hcc->assembly_output += this->name + ":\n";
	hcc->assembly_output += hcc->backend->emit_function_prologue() + "\n";

	AstNode::assemble(hcc);

	hcc->currentFunction.name = "";
	hcc->currentFunction.variables.clear();
}

void hcc::AstReturnNode::assemble(HCC* hcc) {
	return_expression->assemble(hcc);

	Value& top = hcc->backend->values.top();
	if (top.getRegisterName() != hcc->backend->abi.return_register && top.isRegister()) {
		hcc->backend->emit_move(hcc->backend->abi.return_register, top.getRegisterName());
	}

	hcc->assembly_output += hcc->backend->emit_function_epilogue() + "\n";
}

void hcc::AstReturnNode::print() {
	fmt::printf("return;\n");
	AstNode::print();
}

hcc::AstBinaryOpNode* hcc::AstBinaryOpNode::create([[maybe_unused]] Parser* parser, Operation op, AstNode* left, AstNode* right) {
	AstBinaryOpNode* me = new AstBinaryOpNode();
	me->op = op;
	me->left = left;
	me->right = right;
	return me;
}

void hcc::AstBinaryOpNode::print() {
	fmt::printf("(");
	left->print();
	fmt::printf(" %s ", op == Operation::ADD ? "+" : "*");
	right->print();
	fmt::printf(")");
	AstNode::print();
}

void hcc::AstBinaryOpNode::assemble(HCC* hcc) {
	Value LHS, RHS;

	left->assemble(hcc);
	right->assemble(hcc);

	RHS = hcc->backend->values.top();
	hcc->backend->values.pop();
	LHS = hcc->backend->values.top();
	hcc->backend->values.pop();

	switch (op) {
	case Operation::ADD: {
		LHS.add(hcc, &RHS);
	} break;
	case Operation::SUB: {
		LHS.sub(hcc, &RHS);
	} break;
	case Operation::MUL: {
		LHS.mul(hcc, &RHS);
	} break;
	case Operation::DIV: {
		LHS.div(hcc, &RHS);
	} break;
	}

	hcc->backend->values.push(LHS);
}

hcc::AstNumberNode* hcc::AstNumberNode::create(Parser* parser) {
	AstNumberNode* me = new AstNumberNode();
	me->value = std::get<int>(parser->currentToken.value);
	return me;
}

void hcc::AstNumberNode::print() {
	fmt::printf("%d", value);
	AstNode::print();
}

void hcc::AstNumberNode::assemble(HCC* hcc) {
	hcc->assembly_output += hcc->backend->emit_mov_const(this->value);
}

hcc::AstReturnNode* hcc::AstReturnNode::create(Parser* parser) {
	AstReturnNode* me = new AstReturnNode();

	parser->currentToken = parser->lexer.next(); // consume 'return'
	me->return_expression = parser->parseExpression();
	if (!me->return_expression) {
		delete me;
		return nullptr;
	}

	/*Lexer::Token next = parser->lexer.next();
	if (next.type != Lexer::TokenType::SEMICOLON) {
		fmt::print("[hcc] [{}] expected semicolon after return {}\n", next.line, (int)next.type);
		// delete me;
		// return nullptr;
	}*/

	return me;
}
