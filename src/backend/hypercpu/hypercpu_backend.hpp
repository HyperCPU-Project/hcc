#pragma once
#define BACKEND_HYPERCPU
#include <backend/backend.hpp>

class HyperCPUBackend : public Backend {
public:
	HyperCPUBackend();

	/*
	std::string emit_function_prologue();
	std::string emit_function_epilogue();
	std::string emit_mov_const(int32_t constant);
	std::string emit_add(int32_t ROUT, int32_t RLHS, int32_t RRHS);
	std::string emit_mul(int32_t ROUT, int32_t RLHS, int32_t RRHS);
	*/
};
