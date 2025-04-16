#include <backend/backend.hpp>
#include <ncc.hpp>

void NCC::compile() {
	assembly_output += backend->emit_entrypoint();
	parser.root_node->assemble(this);
}
