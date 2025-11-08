#include <ast/ast.hpp>
#include <hcc.hpp>

using namespace hcc;

void AstReturn::Print(int indent) const {
  PrintIndent(indent);
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

  IrOpcode op;

  if (expr) {
    if (!expr->Compile(hcc))
      return false;
    op.ret.has_value = true;
  }

  op.type = IrOpcode::IR_RET;

  hcc->ir.Add(op);

  return true;
}
