#include "ast.hpp"
#include <hcc.hpp>
#include <pch.hpp>

using namespace hcc;

AstNode::~AstNode() {
  for (AstNode* node : children) {
    delete node;
  }
}

bool AstNode::Compile(HCC* hcc) {
  for (AstNode* node : children) {
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
