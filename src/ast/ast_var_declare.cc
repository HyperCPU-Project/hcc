#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstVarDeclare::print(int indent) const {
	printIndent(indent);
	std::cout << "AstVarDeclare" << std::endl;
	printIndent(indent + 1);
	std::cout << "name: " << name << std::endl;
	printIndent(indent + 1);
	std::cout << "type: " << type << std::endl;
}

bool AstVarDeclare::compile(HCC* hcc) {
	TypeMetadata* var_type = hcc->backend->get_type_from_name(type);
	if (!var_type)
		return false;

	IrOpcode op;
	op.type = IrOpcode::IR_ALLOCA;
	op.alloca.name = name;
	op.alloca.md = *var_type;

	hcc->ir.add(op);

	return true;
}
