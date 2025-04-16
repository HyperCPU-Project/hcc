#pragma once
#include <metadata.hpp>
#include <pch.hpp>

namespace hcc {
class HCC;

class Value {
private:
	std::string reg_name;

public:
	VariableMetadata var;

	Value(std::string reg_name);
	Value(VariableMetadata var);
	Value();

	bool isRegister();
	std::string getRegisterName();

	void add(HCC* hcc, Value* other);
	void sub(HCC* hcc, Value* other);
	void mul(HCC* hcc, Value* other);
	void div(HCC* hcc, Value* other);
	void eq(HCC* hcc, Value* other);
};
} // namespace hcc