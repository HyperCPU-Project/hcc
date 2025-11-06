#include <ast/ast.hpp>
#include <hcc.hpp>

using namespace hcc;

void AstVarAssign::print(int indent) const {
  printIndent(indent);
  std::cout << "AstVarAssign" << std::endl;
  printIndent(indent + 1);
  std::cout << "name: " << name << std::endl;
  printIndent(indent + 1);
  std::cout << "expr:" << std::endl;
  expr->print(indent + 2);
}

AstVarAssign::~AstVarAssign() {
  delete expr;
}

bool AstVarAssign::compile(HCC* hcc) {
  hcc->ir.add_reset();
  hcc->ir.add_line();

  if (!expr->compile(hcc))
    return false;

  IrOpcode op;
  op.type = IrOpcode::IR_ASSIGN;
  op.assign.name = name;

  hcc->ir.add(op);

  return true;
}
