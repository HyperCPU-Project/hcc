#pragma once

#include <backend/backend.hpp>

namespace hcc {
class QprocBackend : public Backend {
public:
	QprocBackend();

	uint64_t increment_reg_index();

	void emit_function_prologue(FILE* out, std::string name) override;
	void emit_function_epilogue(FILE* out) override;

	std::string emit_mov_const(FILE* out, uint64_t value, std::string reg_name = "") override;

	void emit_add(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS) override;
	void emit_sub(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS) override;
	void emit_mul(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS) override;
	void emit_div(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS) override;

	void emit_move(FILE* out, std::string rdest, std::string rsrc) override;

	void emit_reserve_stack_space(FILE* out, uint64_t size) override;
	void emit_store_from_stack(FILE* out, uint64_t align, std::string rsrc) override;

	std::string emit_load_from_stack(FILE* out, uint64_t align) override;
	std::string emit_loadaddr_from_stack(FILE* out, uint64_t align) override;
};
} // namespace hcc
