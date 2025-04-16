#pragma once
#include <backend/abi.h>
#include <metadata.hpp>
#include <pch.hpp>
#include <value.hpp>

namespace hcc {
class Backend {
protected:
	std::map<std::string, TypeMetadata> types;

public:
	std::stack<Value> values;
	AbiMetadata abi;

	Backend();
	virtual ~Backend();

	virtual std::string emit_function_prologue() = 0;
	virtual std::string emit_function_epilogue() = 0;
	virtual std::string emit_mov_const(int32_t constant) = 0;
	virtual std::string emit_add(std::string ROUT, std::string RLHS, std::string RRHS) = 0;
	virtual std::string emit_sub(std::string ROUT, std::string RLHS, std::string RRHS) = 0;
	virtual std::string emit_mul(std::string ROUT, std::string RLHS, std::string RRHS) = 0;
	virtual std::string emit_div(std::string ROUT, std::string RLHS, std::string RRHS) = 0;
	virtual std::string emit_move(std::string rdest, std::string rsrc) = 0;
	virtual std::string emit_reserve_stack_space(uint64_t bytes) = 0;

	virtual std::string emit_entrypoint() = 0;

	TypeMetadata* getTypeFromName(std::string name);
};
} // namespace hcc

#include <backend/qproc/qproc_backend.hpp>
