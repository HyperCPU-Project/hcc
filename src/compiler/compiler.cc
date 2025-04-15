#include <ncc.hpp>

void NCC::compile() {
	parser.root_node->assemble(this);
}
