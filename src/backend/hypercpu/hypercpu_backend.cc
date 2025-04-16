#include <backend/hypercpu/hypercpu_backend.hpp>

using namespace hcc;

HyperCPUBackend::HyperCPUBackend() {
	types["void"] = TypeMetadata("void", 0);
	types["int"] = TypeMetadata("int", 4);
	types["long"] = TypeMetadata("long", 8);
}

std::string HyperCPUBackend::emit_entrypoint() {
	return R"(
.attr(entry) hcc_start_main:
  // TODO: Call to main
  halt;

)";
}

std::string HyperCPUBackend::emit_function_prologue() {
	return "";
}

std::string HyperCPUBackend::emit_function_epilogue() {
	return "";
}

std::string HyperCPUBackend::emit_mov_const(int32_t constant) {
	return "";
}

std::string HyperCPUBackend::emit_add(std::string ROUT, std::string RLHS, std::string RRHS) {
	return "";
}

std::string HyperCPUBackend::emit_sub(std::string ROUT, std::string RLHS, std::string RRHS) {
	return "";
}

std::string HyperCPUBackend::emit_mul(std::string ROUT, std::string RLHS, std::string RRHS) {
	return "";
}

std::string HyperCPUBackend::emit_div(std::string ROUT, std::string RLHS, std::string RRHS) {
	return "";
}

std::string HyperCPUBackend::emit_move(std::string rdest, std::string rsrc) {
	return "";
}

std::string HyperCPUBackend::emit_reserve_stack_space([[maybe_unused]] uint64_t bytes) {
	return "";
}
