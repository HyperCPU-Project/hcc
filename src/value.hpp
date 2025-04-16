#pragma once
#include <pch.hpp>

namespace hcc {
class HCC;

class Value {
private:
	std::string reg_name;
	uint64_t stack_align;

public:
	Value(std::string reg_name);
	Value(uint64_t stack_align);
	Value();

	bool isRegister();
	std::string getRegisterName();

	void add(HCC* hcc, Value* other);
	void sub(HCC* hcc, Value* other);
	void mul(HCC* hcc, Value* other);
	void div(HCC* hcc, Value* other);
};
} // namespace hcc