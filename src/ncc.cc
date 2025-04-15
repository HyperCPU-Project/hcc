#include <backend/backend.hpp>
#include <ncc.hpp>
#include <util.hpp>

NCC::NCC() : parser(lexer, nullptr), backend(nullptr) {
#ifdef BACKEND_QPROC
	backend = new QprocBackend();
#endif
	parser.backend = backend;

	lexer.code = "int main() { return (1 + 1) + 2 * 2; }";

	if (!parser.parse())
		return;

	compile();

	fmt::print("{}\n", assembly_output);
}

NCC::~NCC() {
	if (backend) {
		delete backend;
	}
}