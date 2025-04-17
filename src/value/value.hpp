#pragma once

#include <metadata.hpp>

namespace hcc {
class HCC;

class Value {
private:
	std::string reg_name;

	uint64_t var_stack_align;
	std::string var_name;
	TypeMetadata var_type;

public:
	Value();
	~Value();

	bool isRegister();

	static Value* createAsRegister(HCC* hcc, uint32_t value, std::string reg_name = "");
	static Value* createAsStackVar(HCC* hcc, TypeMetadata type);
};
} // namespace hcc