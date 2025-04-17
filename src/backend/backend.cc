#include <backend/backend.hpp>

using namespace hcc;

Backend::Backend() {
	reg_index = 0;
}

Backend::~Backend() {
}

uint64_t Backend::increment_reg_index() {
	return 0;
}

void Backend::emit_function_prologue(FILE* out, std::string name) {
}

void Backend::emit_function_epilogue(FILE* out) {
}

std::string Backend::emit_mov_const(FILE* out, uint64_t value, std::string reg_name) {
	return "";
}

void Backend::emit_add(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::emit_sub(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::emit_mul(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::emit_div(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::emit_move(FILE* out, std::string rdest, std::string rsrc) {
}

void Backend::emit_reserve_stack_space(FILE* out, uint64_t size) {
}

TypeMetadata* Backend::get_type_from_name(std::string name) {
	if (!types.contains(name)) {
		fmt::print("[hcc] unknown type {}\n", name);
		return nullptr;
	}

	return &types[name];
}