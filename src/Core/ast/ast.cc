#include "ast.hpp"
#include <hcc.hpp>
#include <pch.hpp>

using namespace hcc;

AstNode::~AstNode() {
  for (AstNode* node : children) {
    delete node;
  }
}

bool AstNode::compile(HCC* hcc) {
  for (AstNode* node : children) {
    if (!node->compile(hcc))
      return false;
  }
  return true;
}

void AstNode::printIndent(int indent) const {
  for (int i = 0; i < indent; ++i) {
    std::cout << "  ";
  }
}