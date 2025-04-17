#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstReturn::print(int indent) const {
	printIndent(indent);
	std::cout << "AstReturn" << std::endl;
	if (expr) {
		expr->print(indent + 1);
	}
}

AstReturn::~AstReturn() {
	delete expr;
}

bool AstReturn::compile(HCC* hcc) {
	if (!expr->compile(hcc))
		return false;
	Value* ret = hcc->values.top();
	hcc->values.pop();

	// TODO: Fix this ABI independent shi
	if (ret->reg_name != "r0") {
		hcc->backend->emit_move(hcc->getOutFd(), "r0", ret->reg_name);
	}

	return true;
}