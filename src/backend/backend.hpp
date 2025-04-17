#pragma once

#include <metadata.hpp>
#include <pch.hpp>

namespace hcc {
class Backend {
protected:
	uint64_t reg_index;

public:
	std::map<std::string, TypeMetadata> types;

	Backend();
	virtual ~Backend() = 0;

	uint64_t increment_reg_index();

	virtual void emit_function_prologue(FILE* out, std::string name);
	virtual void emit_function_epilogue(FILE* out);

	virtual std::string emit_mov_const(FILE* out, uint64_t value, std::string reg_name = "");

	virtual void emit_add(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS);
	virtual void emit_sub(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS);
	virtual void emit_mul(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS);
	virtual void emit_div(FILE* out, std::string ROUT, std::string RLHS, std::string RRHS);

	virtual void emit_move(FILE* out, std::string rdest, std::string rsrc);
};
} // namespace hcc