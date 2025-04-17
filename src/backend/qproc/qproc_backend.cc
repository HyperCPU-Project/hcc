#include <backend/qproc/qproc_backend.hpp>

using namespace hcc;

QprocBackend::QprocBackend() {
	reg_index = 0;
	types["void"] = TypeMetadata{"void", 0};
	types["int"] = TypeMetadata{"int", 4};
	types["long"] = TypeMetadata{"long", 4}; // size of 4 is intentional here
	abi.return_register = "r0";
}

uint64_t QprocBackend::increment_reg_index() {
	uint64_t res = ++reg_index;
	if (reg_index > 12) {
		reg_index = 0;
	}
	return res;
}

void QprocBackend::emit_function_prologue(FILE* out, std::string name) {
	fmt::fprintf(out, "%s:\n", name);
	fmt::fprintf(out, "push bp\nmov bp sp\n");
}

void QprocBackend::emit_function_epilogue(FILE* out) {
	fmt::fprintf(out, "mov sp bp\npop bp\npop ip\n");
}

std::string QprocBackend::emit_mov_const(FILE* out, uint64_t value, std::string reg_name) {
	if (reg_name == "") {
		reg_name = fmt::format("r{}", increment_reg_index());
	}

	fmt::fprintf(out, "movi %s %ld\n", reg_name, value);

	return reg_name;
}

void QprocBackend::emit_add(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS) {
	if (ROUT != RLHS) {
		fmt::fprintf(out, "add %s %s\nmov %s %s\n", RLHS, RRHS, ROUT, RLHS);
		return;
	}
	fmt::fprintf(out, "add %s %s\n", RLHS, RRHS);
}

void QprocBackend::emit_sub(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS) {
	if (ROUT != RLHS) {
		fmt::fprintf(out, "sub %s %s\nmov %s %s\n", RLHS, RRHS, ROUT, RLHS);
		return;
	}
	fmt::fprintf(out, "sub %s %s\n", RLHS, RRHS);
}

void QprocBackend::emit_mul(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS) {
	if (ROUT != RLHS) {
		fmt::fprintf(out, "mul %s %s\nmov %s %s\n", RLHS, RRHS, ROUT, RLHS);
		return;
	}
	fmt::fprintf(out, "mul %s %s\n", RLHS, RRHS);
}

void QprocBackend::emit_div(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS) {
	if (ROUT != RLHS) {
		fmt::fprintf(out, "div %s %s\nmov %s %s\n", RLHS, RRHS, ROUT, RLHS);
		return;
	}
	fmt::fprintf(out, "div %s %s\n", RLHS, RRHS);
}

void QprocBackend::emit_move(FILE* out, std::string rdest, std::string rsrc) {
	fmt::fprintf(out, "mov %s %s\n", rdest, rsrc);
}