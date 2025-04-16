#include <backend/backend.hpp>
#include <hcc.hpp>
#include <parser/ast.hpp>
#include <parser/parser.hpp>
#include <value.hpp>

using namespace hcc;

AstBinaryOpNode* AstBinaryOpNode::create([[maybe_unused]] Parser* parser, Operation op, AstNode* left, AstNode* right) {
	AstBinaryOpNode* me = new AstBinaryOpNode();
	me->op = op;
	me->left = left;
	me->right = right;
	return me;
}

void AstBinaryOpNode::assemble(HCC* hcc) {
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

void AstBinaryOpNode::print() {
	fmt::printf("(");
	left->print();
	fmt::printf(" %s ", op == Operation::ADD ? "+" : "*");
	right->print();
	fmt::printf(")");
	AstNode::print();
}