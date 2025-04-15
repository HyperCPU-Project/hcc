#include <backend/qproc/qproc_backend.hpp>

QprocBackend::QprocBackend() {
	types["void"] = TypeMetadata("void", 0);
}

std::string QprocBackend::emit_function_prologue() {
	return "push bp\nmov bp sp";
}

std::string QprocBackend::emit_function_epilogue() {
	return "mov bp sp\npop bp\npop ip";
}