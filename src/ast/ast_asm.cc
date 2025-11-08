#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstAsm::Print(int indent) const {
  PrintIndent(indent);
  std::cout << "AstAsm" << std::endl;
}

bool AstAsm::Compile(HCC* hcc) {
  IrOpcode op;
  op.type = IrOpcode::IR_ASM;
  op.asm_.code = code;

  hcc->ir.Add(op);
  return true;
}
