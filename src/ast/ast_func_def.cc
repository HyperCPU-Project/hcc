#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstFuncDef::print(int indent) const {
	printIndent(indent);
	std::cout << "AstFuncDef" << std::endl;
	printIndent(indent + 1);
	std::cout << "args:" << std::endl;
	for (auto& [arg_name, arg_type] : args) {
		printIndent(indent + 2);
		std::cout << arg_name << ": " << arg_type << std::endl;
	}
	printIndent(indent + 1);
	std::cout << "name: " << name << std::endl;
	for (const auto& stmt : children) {
		stmt->print(indent + 1);
	}
}

bool AstFuncDef::compile(HCC* hcc) {
	hcc->backend->emit_function_prologue(name);

	unsigned int i = 0;
	for (auto& [arg_name, arg_type] : args) {
		if (i > hcc->backend->abi.args_registers.size()) {
			hcc_compile_error = fmt::format("{} arguments exceeds the amount that the target machine can handle", i);
			return false;
		}

		TypeMetadata* type_md = hcc->backend->get_type_from_name(arg_type);
		if (!type_md)
			return false;

		auto var = std::unique_ptr<Value>(Value::createAsStackVar(hcc, *type_md));
		auto reg_var = Value();
		reg_var.reg_name = hcc->backend->abi.args_registers[i];

		var->setto(hcc, &reg_var);

		hcc->current_function.variables[arg_name] = std::move(var);
		i++;
	}

	hcc->backend->output += "\n";

	if (!AstNode::compile(hcc))
		return false;

	return true;
}
