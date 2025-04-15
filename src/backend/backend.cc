#include <backend/backend.hpp>

TypeMetadata::TypeMetadata(std::string name, uint32_t size) : name(name), size(size) {
}

Backend::Backend() {
}

Backend::~Backend() {
}

std::string Backend::emit_function_prologue() {
	return "";
}

std::string Backend::emit_function_epilogue() {
	return "";
}

std::string Backend::emit_mov_const([[maybe_unused]] int32_t constant) {
	return "";
}

std::string Backend::emit_mul([[maybe_unused]] int32_t ROUT, [[maybe_unused]] int32_t RLHS, [[maybe_unused]] int32_t RRHS) {
	return "";
}

std::string Backend::emit_add([[maybe_unused]] int32_t ROUT, [[maybe_unused]] int32_t RLHS, [[maybe_unused]] int32_t RRHS) {
	return "";
}

TypeMetadata* Backend::getTypeFromName(std::string name) {
	if (!types.contains(name)) {
		return nullptr;
	}

	return &types[name];
}