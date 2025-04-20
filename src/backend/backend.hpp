#pragma once

#include <metadata.hpp>
#include <pch.hpp>

namespace hcc {
class Backend {
protected:
	uint64_t reg_index;

public:
	std::string output;
	ABIMetadata abi;

	std::map<std::string, TypeMetadata> types;

	Backend();
	virtual ~Backend() = 0;

	uint64_t increment_reg_index();

	virtual void emit_function_prologue(std::string name);
	virtual void emit_function_epilogue();

	virtual std::string emit_mov_const(uint64_t value, std::string reg_name = "");

	virtual void emit_add(std::string ROUT, std::string RLHS, std::string RRHS);
	virtual void emit_sub(std::string ROUT, std::string RLHS, std::string RRHS);
	virtual void emit_mul(std::string ROUT, std::string RLHS, std::string RRHS);
	virtual void emit_div(std::string ROUT, std::string RLHS, std::string RRHS);

	virtual void emit_move(std::string rdest, std::string rsrc);

	virtual void emit_reserve_stack_space(uint64_t size);

	virtual std::string emit_load_from_stack(uint64_t align, std::string load_reg = "");
	virtual void emit_store_from_stack(uint64_t align, std::string rsrc);
	virtual std::string emit_loadaddr_from_stack(uint64_t align, std::string load_reg = "");

	virtual void emit_call(std::string name);

	virtual void emit_push(std::string reg);
	virtual void emit_pop(std::string reg);

	virtual TypeMetadata* get_type_from_name(std::string name);
};
} // namespace hcc
