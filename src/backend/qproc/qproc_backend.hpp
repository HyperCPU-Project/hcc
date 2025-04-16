#pragma once
#define BACKEND_QPROC
#include <backend/backend.hpp>

class QprocBackend : public Backend {
public:
	uint8_t reg_index;
	std::stack<uint8_t> previous_reg_indexes;

public:
	QprocBackend();

	std::string emit_function_prologue();
	std::string emit_function_epilogue();
	std::string emit_mov_const(int32_t constant);
	std::string emit_add(std::string ROUT, std::string RLHS, std::string RRHS);
	std::string emit_mul(std::string ROUT, std::string RLHS, std::string RRHS);
};
