#include "ast.hpp"
#include <cstd_pch.hpp>
#include <hcc.hpp>

using namespace hcc;

AstNode::~AstNode() {
}

bool AstNode::Compile(HCC* hcc) {
  for (std::unique_ptr<AstNode>& node : children) {
    if (!node->Compile(hcc))
      return false;
  }
  return true;
}

void AstNode::PrintIndent(int indent) const {
  for (int i = 0; i < indent; ++i) {
    std::cout << "  ";
  }
}
