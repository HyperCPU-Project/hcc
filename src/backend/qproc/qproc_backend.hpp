#pragma once

#include <backend/backend.hpp>

namespace hcc {
class QprocBackend : public Backend {
public:
	QprocBackend();

	void emit_function_prologue(FILE* out, std::string name) override;
	void emit_function_epilogue(FILE* out) override;
};
} // namespace hcc