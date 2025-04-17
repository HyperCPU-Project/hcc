#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstBinaryOp::print(int indent) const {
	printIndent(indent);
	std::cout << "AstBinaryOp" << std::endl;
	printIndent(indent + 1);
	std::cout << "op: " << op << std::endl;
	printIndent(indent + 1);
	std::cout << "left:" << std::endl;
	left->print(indent + 2);
	printIndent(indent + 1);
	std::cout << "right:" << std::endl;
	right->print(indent + 2);
}

AstBinaryOp::~AstBinaryOp() {
	delete left;
	delete right;
}

bool AstBinaryOp::compile(HCC* hcc) {
	if (!left->compile(hcc))
		return false;
	if (!right->compile(hcc))
		return false;

	Value* RHS = hcc->values.top();
	hcc->values.pop();
	Value* LHS = hcc->values.top();
	hcc->values.pop();

	if (op == "add") {
		LHS->add(hcc, RHS);
	} else if (op == "sub") {
		LHS->sub(hcc, RHS);
	} else if (op == "mul") {
		LHS->mul(hcc, RHS);
	} else if (op == "div") {
		LHS->div(hcc, RHS);
	}

	hcc->values.push(LHS);

	return true;
}