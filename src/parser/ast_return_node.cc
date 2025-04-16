#include <backend/backend.hpp>
#include <hcc.hpp>
#include <parser/ast.hpp>
#include <parser/parser.hpp>
#include <value.hpp>

using namespace hcc;

AstReturnNode* AstReturnNode::create(Parser* parser) {
	AstReturnNode* me = new AstReturnNode();

	parser->currentToken = parser->lexer.next(); // consume 'return'
	me->return_expression = parser->parseExpression();
	if (!me->return_expression) {
		delete me;
		return nullptr;
	}

	return me;
}

void AstReturnNode::assemble(HCC* hcc) {
	return_expression->assemble(hcc);

	Value& top = hcc->backend->values.top();
	if (top.getRegisterName() != hcc->backend->abi.return_register && top.isRegister()) {
		hcc->backend->emit_move(hcc->backend->abi.return_register, top.getRegisterName());
	}

	hcc->assembly_output += hcc->backend->emit_function_epilogue() + "\n";
}

void AstReturnNode::print() {
	fmt::printf("return;\n");
	AstNode::print();
}