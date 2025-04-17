#include <backend/qproc/qproc_backend.hpp>

using namespace hcc;

QprocBackend::QprocBackend() {
	types["void"] = TypeMetadata{"void", 0};
	types["int"] = TypeMetadata{"int", 4};
	types["long"] = TypeMetadata{"long", 4}; // size of 4 is intentional here
}

void QprocBackend::emit_function_prologue(FILE* out, std::string name) {
	fmt::fprintf(out, "%s:\n", name);
	fmt::fprintf(out, "push bp\nmov bp sp\n");
}

void QprocBackend::emit_function_epilogue(FILE* out) {
	fmt::fprintf(out, "mov sp bp\npop bp\npop ip\n");
}