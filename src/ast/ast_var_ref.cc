#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstVarRef::print(int indent) const {
  printIndent(indent);
  std::cout << name << std::endl;
}

bool AstVarRef::compile(HCC* hcc) {
  IrOpcode op;
  op.type = IrOpcode::IR_VARREF;
  op.varref.name = name;

  hcc->ir.add(op);

  return true;
}
