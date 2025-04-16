#pragma once
#define BACKEND_HYPERCPU
#include <backend/backend.hpp>

namespace hcc {
class HyperCPUBackend : public Backend {
public:
	HyperCPUBackend();

	std::string emit_function_prologue();
	std::string emit_function_epilogue();
	std::string emit_mov_const(int32_t constant);
	std::string emit_add(std::string ROUT, std::string RLHS, std::string RRHS);
	std::string emit_sub(std::string ROUT, std::string RLHS, std::string RRHS);
	std::string emit_mul(std::string ROUT, std::string RLHS, std::string RRHS);
	std::string emit_div(std::string ROUT, std::string RLHS, std::string RRHS);
	std::string emit_move(std::string rdest, std::string rsrc);

	std::string emit_entrypoint();
};
} // namespace hcc