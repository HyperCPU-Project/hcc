#pragma once
#include <metadata.hpp>
#include <pch.hpp>
#include <value.hpp>

class Backend {
protected:
	std::map<std::string, TypeMetadata> types;

public:
	std::stack<Value> values;

	Backend();
	virtual ~Backend();

	virtual std::string emit_function_prologue() = 0;
	virtual std::string emit_function_epilogue() = 0;
	virtual std::string emit_mov_const(int32_t constant) = 0;
	virtual std::string emit_add(std::string ROUT, std::string RLHS, std::string RRHS) = 0;
	virtual std::string emit_mul(std::string, std::string RLHS, std::string RRHS) = 0;

	virtual std::string emit_entrypoint() = 0;

	TypeMetadata* getTypeFromName(std::string name);
};

#include <backend/qproc/qproc_backend.hpp>
