#include <ast/ast.hpp>

using namespace hcc;

void AstVarAssign::print(int indent) const {
	printIndent(indent);
	std::cout << "AstVarAssign" << std::endl;
	printIndent(indent + 1);
	std::cout << "name: " << name << std::endl;
	printIndent(indent + 1);
	std::cout << "expr:" << std::endl;
	expr->print(indent + 2);
}

AstVarAssign::~AstVarAssign() {
	delete expr;
}