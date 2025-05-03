#include "ir/ir.hpp"
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
	IrOpcode op;
	op.type = IrOpcode::IR_FUNCDEF;
	op.funcdef.name = name;

	for (auto& [arg_name, arg_type] : args) {
		op.funcdef.arg_names.push_back(arg_name);

		TypeMetadata* md = hcc->backend->get_type_from_name(arg_type);
		if (!md)
			return false;
		op.funcdef.arg_types.push_back(*md);
	}

	hcc->ir.add(op);

	if (!AstNode::compile(hcc))
		return false;

	return true;
}
