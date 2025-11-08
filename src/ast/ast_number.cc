#include <ast/ast.hpp>
#include <hcc.hpp>
#include <ir/ir.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstNumber::Print(int indent) const {
  PrintIndent(indent);
  std::cout << "AstNumber" << std::endl;
  PrintIndent(indent + 1);
  std::cout << "value: " << value << std::endl;
}

bool AstNumber::Compile(HCC* hcc) {
  IrOpcode op;
  if (hcc->optimizations.HasFlag(Optimization::OPT_CONSTANT_FOLDING)) {
    op.type = IrOpcode::IR_CCTV;
    op.cctv.value = value;
  } else {
    op.type = IrOpcode::IR_CREG;
    op.creg.value = value;
    op.creg.reg_name = "";
  }

  hcc->ir.Add(op);

  return true;
}
