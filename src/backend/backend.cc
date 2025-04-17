#include <backend/backend.hpp>

using namespace hcc;

Backend::Backend() {
}

Backend::~Backend() {
}

void Backend::emit_function_prologue([[maybe_unused]] FILE* out, [[maybe_unused]] std::string name) {
}

void Backend::emit_function_epilogue([[maybe_unused]] FILE* out) {
}