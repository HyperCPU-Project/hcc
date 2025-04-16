#include <backend/backend.hpp>
#include <ncc.hpp>

void NCC::compile() {
	assembly_output += backend->emit_entrypoint();
	if (printAst)
		parser.root_node->print();
	parser.root_node->assemble(this);
}
