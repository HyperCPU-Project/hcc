#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstBinaryOp::Print(int indent) const {
  printIndent(indent);
  std::cout << "AstBinaryOp" << std::endl;
  printIndent(indent + 1);
  std::cout << "op: " << op << std::endl;
  printIndent(indent + 1);
  std::cout << "left:" << std::endl;
  left->Print(indent + 2);
  printIndent(indent + 1);
  std::cout << "right:" << std::endl;
  right->Print(indent + 2);
}

AstBinaryOp::~AstBinaryOp() {
  delete left;
  delete right;
}

bool AstBinaryOp::Compile(HCC* hcc) {
  if (!left->Compile(hcc))
    return false;
  if (!right->Compile(hcc))
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

  hcc->ir.Add(ir_op);

  return true;
}
