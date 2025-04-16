#include <backend/backend.hpp>
#include <ncc.hpp>
#include <util.hpp>

NCC::NCC() : parser(lexer, nullptr), backend(nullptr) {
#ifdef BACKEND_QPROC
	backend = new QprocBackend();
#endif
	parser.backend = backend;

	lexer.code = "int main() { return 69 / 3; }";

	fmt::print("[ncc] parsing\n");
	if (!parser.parse())
		return;

	fmt::print("[ncc] compiling\n");
	compile();

	fmt::print("{}\n", assembly_output);
}

NCC::~NCC() {
	if (backend) {
		delete backend;
	}
}
