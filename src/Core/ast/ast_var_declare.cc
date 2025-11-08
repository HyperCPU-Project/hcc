#include <ast/ast.hpp>
#include <hcc.hpp>

using namespace hcc;

void AstVarDeclare::Print(int indent) const {
  PrintIndent(indent);
  std::cout << "AstVarDeclare" << std::endl;
  PrintIndent(indent + 1);
  std::cout << "names: ";
  for (std::string name : names) {
    std::cout << name << " ";
  }
  std::cout << std::endl;

  PrintIndent(indent + 1);
  std::cout << "type: " << type << std::endl;
}

bool AstVarDeclare::Compile(HCC* hcc) {
  TypeMetadata* var_type = hcc->backend->GetTypeFromName(type);
  if (!var_type)
    return false;

  for (std::string name : names) {
    IrOpcode op;
    op.type = IrOpcode::IR_ALLOCA;
    op.alloca.name = name;
    op.alloca.md = *var_type;

    hcc->ir.Add(op);
  }

  return true;
}
