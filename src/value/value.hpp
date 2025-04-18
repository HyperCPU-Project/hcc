#pragma once

#include <metadata.hpp>

namespace hcc {
class HCC;

class Value {
public:
	std::string reg_name;

	uint64_t var_stack_align;
	std::string var_name;
	TypeMetadata var_type;

	Value();
	~Value();

	bool isRegister();

	static Value* createAsRegister(HCC* hcc, uint64_t value, std::string reg_name = "");
	static Value* createAsStackVar(HCC* hcc, TypeMetadata type, std::string name = "");

	Value* doCondLod(HCC* hcc);

	void add(HCC* hcc, Value* other);
	void sub(HCC* hcc, Value* other);
	void mul(HCC* hcc, Value* other);
	void div(HCC* hcc, Value* other);
};
} // namespace hcc
