#include <ast/ast.hpp>

void AstVarDeclare::print(int indent) const {
	printIndent(indent);
	std::cout << "AstVarDeclare" << std::endl;
	printIndent(indent + 1);
	std::cout << "name: " << name << std::endl;
	printIndent(indent + 1);
	std::cout << "type: " << type << std::endl;
}