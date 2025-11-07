#include <ast/ast.hpp>

using namespace hcc;

void AstRootNode::Print(int indent) const {
  printIndent(indent);
  std::cout << "AstRootNode" << std::endl;
  for (const auto& child : children) {
    child->Print(indent + 1);
  }
}