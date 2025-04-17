#include <ast/ast.hpp>

void AstVarRef::print(int indent) const {
	printIndent(indent);
	std::cout << name << std::endl;
}