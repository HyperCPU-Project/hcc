#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstReturn::Print(int indent) const {
  printIndent(indent);
  std::cout << "AstReturn" << std::endl;
  if (expr) {
    expr->Print(indent + 1);
  }
}

AstReturn::~AstReturn() {
  delete expr;
}

bool AstReturn::Compile(HCC* hcc) {
  hcc->ir.AddReset();

  if (expr) {
    if (!expr->Compile(hcc))
      return false;
  }

  IrOpcode op;
  op.type = IrOpcode::IR_RET;

  hcc->ir.Add(op);

  return true;
}
