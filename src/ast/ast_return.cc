#include <ast/ast.hpp>
#include <hcc.hpp>

using namespace hcc;

void AstReturn::print(int indent) const {
  printIndent(indent);
  std::cout << "AstReturn" << std::endl;
  if (expr) {
    expr->print(indent + 1);
  }
}

AstReturn::~AstReturn() {
  delete expr;
}

bool AstReturn::compile(HCC* hcc) {
  hcc->ir.add_reset();

  IrOpcode op;

  if (expr) {
    if (!expr->compile(hcc))
      return false;
    op.ret.has_value = true;
  }

  op.type = IrOpcode::IR_RET;

  hcc->ir.add(op);

  return true;
}
