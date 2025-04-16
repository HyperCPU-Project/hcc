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

	uint32_t declared_at;
} VariableMetadata;

typedef struct FunctionMetadata {
	std::map<std::string, VariableMetadata> variables;
	std::string name;
	TypeMetadata return_type;
	uint64_t stack_space;
} FunctionMetadata;
