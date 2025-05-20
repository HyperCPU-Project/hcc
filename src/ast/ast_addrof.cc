#include <ast/ast.hpp>
#include <hcc.hpp>
#include <ir/ir.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstAddrof::print(int indent) const {
  printIndent(indent);
  std::cout << "&" + name << std::endl;
}

bool AstAddrof::compile(HCC* hcc) {
  IrOpcode op;
  op.type = IrOpcode::IR_ADDROF;
  op.addrof.name = name;

  hcc->ir.add(op);

  return true;
}
