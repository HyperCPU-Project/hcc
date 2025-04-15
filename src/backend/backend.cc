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

TypeMetadata* Backend::getTypeFromName(std::string name) {
	if (!types.contains(name)) {
		return nullptr;
	}

	return &types[name];
}