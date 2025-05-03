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
	/*
	if (expr) {
		if (!expr->compile(hcc))
			return false;
		auto ret_raw = std::move(hcc->values.top());
		hcc->values.pop();

		auto ret = ret_raw->use(hcc);

		if (ret->reg_name != hcc->backend->abi.return_register) {
			hcc->backend->emit_move(hcc->backend->abi.return_register, ret->reg_name);
		}

		if (ret != ret_raw.get())
			delete ret;
	}
	hcc->backend->emit_function_epilogue();
	*/

	if (expr) {
		if (!expr->compile(hcc))
			return false;
	}

	IrOpcode op;
	op.type = IrOpcode::IR_RET;

	hcc->ir.add(op);

	return true;
}
