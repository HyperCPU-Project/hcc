#include "ast.hpp"
#include <iostream>

using namespace hcc;

AstNode::~AstNode() {
	for (AstNode* node : children) {
		delete node;
	}
}

void AstNode::printIndent(int indent) const {
	for (int i = 0; i < indent; ++i) {
		std::cout << "  ";
	}
}