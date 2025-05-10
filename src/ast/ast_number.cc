#include <ast/ast.hpp>
#include <hcc.hpp>
#include <ir/ir.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstNumber::print(int indent) const {
	printIndent(indent);
	std::cout << "AstNumber" << std::endl;
	printIndent(indent + 1);
	std::cout << "value: " << value << std::endl;
}

bool AstNumber::compile(HCC* hcc) {
	IrOpcode op;
	op.type = IrOpcode::IR_CCTV;
	op.cctv.value = value;

	hcc->ir.add(op);

	return true;
}
