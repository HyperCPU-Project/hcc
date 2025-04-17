#include <ast/ast.hpp>
#include <hcc.hpp>

using namespace hcc;

void AstFuncDef::print(int indent) const {
	printIndent(indent);
	std::cout << "AstFuncDef" << std::endl;
	printIndent(indent + 1);
	std::cout << "name: " << name << std::endl;
	for (const auto& stmt : children) {
		stmt->print(indent + 1);
	}
}

bool AstFuncDef::compile(HCC* hcc) {
	hcc->backend->emit_function_prologue(hcc->getOutFd(), name);
	if (!AstNode::compile(hcc))
		return false;
	return true;
}