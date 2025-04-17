#include <ast/ast.hpp>

using namespace hcc;

void AstReturn::print(int indent) const {
	printIndent(indent);
	std::cout << "AstReturn" << std::endl;
	if (expr) {
		expr->print(indent + 1);
	}
}

AstReturn::~AstReturn() {
	delete expr;
}