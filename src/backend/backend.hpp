#pragma once
#include <pch.hpp>

typedef struct TypeMetadata {
	std::string name;
	uint32_t size;

	TypeMetadata(std::string name = "", uint32_t size = 0);
} TypeMetadata;

typedef struct VariableMetadata {
	TypeMetadata type;
	uint32_t stack_align;
	std::string name;
} VariableMetadata;

typedef struct FunctionMetadata {
	std::map<std::string, VariableMetadata> variables;
	std::string name;
	TypeMetadata return_type;
} FunctionMetadata;

class Backend {
protected:
	std::map<std::string, TypeMetadata> types;

public:
	Backend();
	virtual ~Backend();

	virtual std::string emit_function_prologue() = 0;
	virtual std::string emit_function_epilogue() = 0;
	virtual std::string emit_mov_const(int32_t constant) = 0;
	virtual std::string emit_add(int32_t ROUT, int32_t RLHS, int32_t RRHS) = 0;
	virtual std::string emit_mul(int32_t ROUT, int32_t RLHS, int32_t RRHS) = 0;

	TypeMetadata* getTypeFromName(std::string name);
};

#include <backend/qproc/qproc_backend.hpp>