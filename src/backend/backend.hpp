#pragma once

#include <metadata.hpp>
#include <pch.hpp>

namespace hcc {
class Backend {
public:
	std::map<std::string, TypeMetadata> types;

	Backend();
	virtual ~Backend() = 0;

	virtual void emit_function_prologue(FILE* out, std::string name) = 0;
	virtual void emit_function_epilogue(FILE* out) = 0;
};
} // namespace hcc