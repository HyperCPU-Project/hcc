#pragma once
#include <pch.hpp>

typedef struct VariableMetadata {
	uint32_t size;
	uint32_t stack_align;
	std::string name;
} VariableMetadata;

typedef struct TypeMetadata {
	std::string name;
	uint32_t size;

	TypeMetadata(std::string name = "", uint32_t size = 0);
} TypeMetadata;

class Backend {
protected:
	std::map<std::string, TypeMetadata> types;

public:
	Backend();
	virtual ~Backend();

	virtual std::string emit_function_prologue() = 0;
	virtual std::string emit_function_epilogue() = 0;

	TypeMetadata* getTypeFromName(std::string name);
};