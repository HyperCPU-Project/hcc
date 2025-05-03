#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstBinaryOp::print(int indent) const {
	printIndent(indent);
	std::cout << "AstBinaryOp" << std::endl;
	printIndent(indent + 1);
	std::cout << "op: " << op << std::endl;
	printIndent(indent + 1);
	std::cout << "left:" << std::endl;
	left->print(indent + 2);
	printIndent(indent + 1);
	std::cout << "right:" << std::endl;
	right->print(indent + 2);
}

AstBinaryOp::~AstBinaryOp() {
	delete left;
	delete right;
}

bool AstBinaryOp::compile(HCC* hcc) {
	if (!left->compile(hcc))
		return false;
	if (!right->compile(hcc))
		return false;

	IrOpcode ir_op;

	if (op == "add") {
		ir_op.type = IrOpcode::IR_ADD;
	} else if (op == "sub") {
		ir_op.type = IrOpcode::IR_SUB;
	} else if (op == "mul") {
		ir_op.type = IrOpcode::IR_MUL;
	} else if (op == "div") {
		ir_op.type = IrOpcode::IR_DIV;
	}

	hcc->ir.add(ir_op);

	return true;
}
