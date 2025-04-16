#include <backend/backend.hpp>
#include <hcc.hpp>

bool hcc::HCC::compile() {
	assembly_output += backend->emit_entrypoint();
	if (printAst)
		parser.root_node->print();
	if (!parser.root_node->assemble(this))
		return false;
	return true;
}
