#include <ast/ast.hpp>
#include <hcc.hpp>
#include <ir/ir.hpp>

using namespace hcc;

void AstAddrof::Print(int indent) const {
  PrintIndent(indent);
  std::cout << "&" + name << std::endl;
}

bool AstAddrof::Compile(HCC* hcc) {
  IrOpcode op;
  op.type = IrOpcode::IR_ADDROF;
  op.addrof.name = name;

  hcc->ir.Add(op);

  return true;
}
