#include "ast.hpp"
#include <cstd_pch.hpp>
#include <hcc.hpp>

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
