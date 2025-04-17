#include <ast/ast.hpp>

using namespace hcc;

void AstVarRef::print(int indent) const {
	printIndent(indent);
	std::cout << name << std::endl;
}