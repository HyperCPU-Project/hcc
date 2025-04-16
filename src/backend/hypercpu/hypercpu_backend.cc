#include <backend/hypercpu/hypercpu_backend.hpp>

HyperCPUBackend::HyperCPUBackend() {
	types["void"] = TypeMetadata("void", 0);
	types["int"] = TypeMetadata("int", 4);
	types["long"] = TypeMetadata("long", 8);
}

std::string HyperCPUBackend::emit_entrypoint() {
	return R"(
.attr(entry) ncc_start_main:
  // TODO: Call to main
  halt;

)";
}

/*
std::string HyperCPUBackend::emit_function_prologue() {
	return "push bp\nmov bp sp\n";
}

std::string HyperCPUBackend::emit_function_epilogue() {
	return "mov bp sp\npop bp\npop ip\n";
}

std::string HyperCPUBackend::emit_mov_const(int32_t constant) {
	std::string code = fmt::format("movi r{} {}\n", reg_index, constant);
	previous_reg_indexes.push(reg_index);
	reg_index++;
	if (reg_index > 12) {
		reg_index = 0;
	}
	return code;
}

std::string HyperCPUBackend::emit_add(int32_t ROUT, int32_t RLHS, int32_t RRHS) {
	return fmt::format("movi r{} r{}\nadd r{} r{}\n", ROUT, RLHS, ROUT, RRHS);
}

std::string HyperCPUBackend::emit_mul(int32_t ROUT, int32_t RLHS, int32_t RRHS) {
	return fmt::format("movi r{} r{}\nmul r{} r{}\n", ROUT, RLHS, ROUT, RRHS);
}*/
