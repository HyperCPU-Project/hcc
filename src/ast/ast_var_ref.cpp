#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstVarRef::Print(int indent) const {
  PrintIndent(indent);
  std::cout << name << std::endl;
}

bool AstVarRef::Compile(HCC* hcc) {
  IrOpcode op;
  op.type = IrOpcode::IR_VARREF;
  op.varref.name = name;

  hcc->ir.Add(op);

  return true;
}
