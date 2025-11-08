#include <ast/ast.hpp>
#include <hcc.hpp>
#include <ir/ir.hpp>

using namespace hcc;

void AstNumber::Print(int indent) const {
  PrintIndent(indent);
  std::cout << "AstNumber" << std::endl;
  PrintIndent(indent + 1);
  std::cout << "value: " << value << std::endl;
}

bool AstNumber::Compile(HCC* hcc) {
  IrOpcode op;
  op.type = IrOpcode::IR_CCTV;
  op.cctv.value = value;

  hcc->ir.Add(op);

  return true;
}
