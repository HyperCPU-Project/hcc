#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstAsm::print(int indent) const {
  printIndent(indent);
  std::cout << "AstAsm" << std::endl;
}

bool AstAsm::compile(HCC* hcc) {
  IrOpcode op;
  op.type = IrOpcode::IR_ASM;
  op.asm_.code = code;

  hcc->ir.add(op);
  return true;
}
