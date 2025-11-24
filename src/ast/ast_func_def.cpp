#include "ir/ir.hpp"
#include <ast/ast.hpp>
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

void AstFuncDef::Print(int indent) const {
  PrintIndent(indent);
  std::cout << "AstFuncDef" << std::endl;
  PrintIndent(indent + 1);
  std::cout << "args:" << std::endl;
  for (auto& [arg_name, arg_type] : args) {
    PrintIndent(indent + 2);
    std::cout << arg_name << ": " << arg_type << std::endl;
  }
  PrintIndent(indent + 1);
  std::cout << "name: " << name << std::endl;
  for (const auto& stmt : children) {
    stmt->Print(indent + 1);
  }
}

bool AstFuncDef::Compile(HCC* hcc) {
  IrOpcode op;
  op.type = IrOpcode::IR_FUNCDEF;
  op.funcdef.name = name;

  for (auto& [arg_name, arg_type] : args) {
    fmt::println("arg {}", arg_name);
    op.funcdef.arg_names.push_back(arg_name);

    TypeMetadata* md = hcc->backend->GetTypeFromName(arg_type);
    if (!md)
      return false;
    op.funcdef.arg_types.push_back(*md);
  }

  hcc->ir.Add(op);

  if (!AstNode::Compile(hcc))
    return false;

  return true;
}
