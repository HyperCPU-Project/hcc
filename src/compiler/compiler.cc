#include <ncc.hpp>

void NCC::compile() {
	for (AstNode* node : parser.root_node->children) {
		node->assemble(this);
	}
}