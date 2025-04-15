#pragma once
#include <backend/backend.hpp>

class QprocBackend : public Backend {
public:
	QprocBackend();

	std::string emit_function_prologue();
	std::string emit_function_epilogue();
};