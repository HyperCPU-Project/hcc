#include <backend/backend.hpp>
#include <parser/ast.hpp>
#include <value.hpp>

void hcc::AstNode::print() {
	for (AstNode* node : children) {
		node->print();
	}
}

void hcc::AstNode::assemble([[maybe_unused]] HCC* hcc) {
	for (AstNode* node : children) {
		node->assemble(hcc);
	}
}

hcc::AstNode::~AstNode() {
	for (AstNode* child : children) {
		delete child;
	}
}

void hcc::AstRootNode::print() {
	fmt::printf("(root node)\n");
	AstNode::print();
}