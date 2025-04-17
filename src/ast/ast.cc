#include "ast.hpp"
#include <iostream>

AstNode::~AstNode() {
	printf("destroy\n");
	for (AstNode* node : children) {
		delete node;
	}
}

void AstNode::printIndent(int indent) const {
	for (int i = 0; i < indent; ++i) {
		std::cout << "  ";
	}
}