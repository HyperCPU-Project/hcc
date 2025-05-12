#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstFuncCall::print(int indent) const {
	printIndent(indent);
	std::cout << "AstFuncCall" << std::endl;
	printIndent(indent + 1);
	std::cout << "name: " << name << std::endl;
	for (AstNode* arg : args) {
		arg->print(indent + 1);
	}
}

AstFuncCall::~AstFuncCall() {
	for (AstNode* arg : args) {
		delete arg;
	}
}

bool AstFuncCall::compile(HCC* hcc) {
	/*
	std::vector<std::string> pushed_regs;
	for (unsigned int i = 0; i < args.size() && i < hcc->backend->abi.args_registers.size(); i++) {
		if (!args[i]->compile(hcc))
			return false;
		std::string neededRegister = hcc->backend->abi.args_registers[i];

		auto arg = std::move(hcc->values.top());
		hcc->values.pop();

		Value* v = arg->doCondLod(hcc, neededRegister);
		if (v != arg.get())
			delete v;

		hcc->backend->emit_push(v->reg_name);
		pushed_regs.insert(pushed_regs.begin(), neededRegister);
	}

	for (std::string reg : pushed_regs) {
		hcc->backend->emit_pop(reg);
	}

	hcc->backend->emit_call(name);

	auto ret = std::make_unique<Value>();
	ret->reg_name = hcc->backend->abi.return_register;
	hcc->values.push(std::move(ret));
	*/

	IrOpcode op;
	op.type = IrOpcode::IR_CALL;
	op.call.name = name;

	hcc->ir.add(op);

	return true;
}
