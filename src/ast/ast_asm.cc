#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstAsm::print(int indent) const {
	printIndent(indent);
	std::cout << "AstAsm" << std::endl;
}

bool AstAsm::compile(HCC* hcc) {
	fmt::fprintf(hcc->getOutFd(), "%s\n\n", code);
	return true;
}
