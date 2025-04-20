#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstAsm::print(int indent) const {
	printIndent(indent);
	std::cout << "AstAsm" << std::endl;
}

bool AstAsm::compile(HCC* hcc) {
	hcc->backend->output = code + "\n\n";
	return true;
}
