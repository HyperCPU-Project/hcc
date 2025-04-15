#include <backend/qproc/qproc_backend.hpp>

QprocBackend::QprocBackend() {
	types["void"] = TypeMetadata("void", 0);
	types["int"] = TypeMetadata("int", 4);
	reg_index = 0;
}

std::string QprocBackend::emit_function_prologue() {
	return "push bp\nmov bp sp\n";
}

std::string QprocBackend::emit_function_epilogue() {
	return "mov bp sp\npop bp\npop ip\n";
}

std::string QprocBackend::emit_mov_const(int32_t constant) {
	return fmt::format("movi r{} {}\n", reg_index, constant);
	previous_reg_indexes.push(reg_index);
	reg_index++;
	if (reg_index > 12) {
		reg_index = 0;
	}
}

std::string QprocBackend::emit_add(int32_t ROUT, int32_t RLHS, int32_t RRHS) {
	return fmt::format("movi r{} r{}\nadd r{} r{}\n", ROUT, RLHS, ROUT, RRHS);
}

std::string QprocBackend::emit_mul(int32_t ROUT, int32_t RLHS, int32_t RRHS) {
	return fmt::format("movi r{} r{}\nmul r{} r{}\n", ROUT, RLHS, ROUT, RRHS);
}