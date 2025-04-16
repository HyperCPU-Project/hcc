#include <backend/backend.hpp>
#include <hcc.hpp>
#include <parser/ast.hpp>
#include <value.hpp>

using namespace hcc;

AstNumberNode* AstNumberNode::create(Parser* parser) {
	AstNumberNode* me = new AstNumberNode();
	me->value = std::get<int>(parser->currentToken.value);
	return me;
}

void AstNumberNode::print() {
	fmt::printf("%d", value);
	AstNode::print();
}

void AstNumberNode::assemble(HCC* hcc) {
	hcc->assembly_output += hcc->backend->emit_mov_const(this->value);
}