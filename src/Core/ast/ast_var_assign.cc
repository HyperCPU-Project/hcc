#include <ast/ast.hpp>
#include <hcc.hpp>

using namespace hcc;

void AstVarAssign::Print(int indent) const {
  PrintIndent(indent);
  std::cout << "AstVarAssign" << std::endl;
  PrintIndent(indent + 1);
  std::cout << "name: " << name << std::endl;
  PrintIndent(indent + 1);
  std::cout << "expr:" << std::endl;
  expr->Print(indent + 2);
}

AstVarAssign::~AstVarAssign() {
  delete expr;
}

bool AstVarAssign::Compile(HCC* hcc) {
  hcc->ir.AddReset();
  hcc->ir.AddLine();

  if (!expr->Compile(hcc))
    return false;

  IrOpcode op;
  op.type = IrOpcode::IR_ASSIGN;
  op.assign.name = name;

  hcc->ir.Add(op);

  return true;
}
