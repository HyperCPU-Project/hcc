#include <ast/ast.hpp>

void AstFuncDef::print(int indent) const {
	printIndent(indent);
	std::cout << "AstFuncDef" << std::endl;
	printIndent(indent + 1);
	std::cout << "name: " << name << std::endl;
	for (const auto& stmt : children) {
		stmt->print(indent + 1);
	}
}