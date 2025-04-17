#include <ast/ast.hpp>

void AstNumber::print(int indent) const {
	printIndent(indent);
	std::cout << "AstNumber" << std::endl;
	printIndent(indent + 1);
	std::cout << "value: " << value << std::endl;
}