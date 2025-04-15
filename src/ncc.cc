#include <backend/qproc/qproc_backend.hpp>
#include <ncc.hpp>
#include <util.hpp>

NCC::NCC() : parser(lexer, nullptr), backend(nullptr) {
	backend = new QprocBackend();
	parser.backend = backend;

	lexer.code = "void main() { return; }";

	parser.parse();
}

NCC::~NCC() {
	if (backend) {
		delete backend;
	}
}