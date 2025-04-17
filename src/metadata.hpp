#pragma once

#include <pch.hpp>

namespace hcc {
class Value;

typedef struct TypeMetadata {
	std::string name;
	uint8_t size; // in bytes
} TypeMetadata;

typedef struct FunctionMetadata {
	std::string name;
	uint64_t align;
	std::map<std::string, Value*> variables;
} FunctionMetadata;
} // namespace hcc