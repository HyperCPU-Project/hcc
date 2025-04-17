#include <ast/ast.hpp>

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