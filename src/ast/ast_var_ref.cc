#include <ast/ast.hpp>
#include <hcc.hpp>

using namespace hcc;

void AstVarRef::print(int indent) const {
	printIndent(indent);
	std::cout << name << std::endl;
}

bool AstVarRef::compile(HCC* hcc) {
	return true;
}