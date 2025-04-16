#pragma once
#include <pch.hpp>

class NCC;
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

	void add(NCC* ncc, Value* other);
	void sub(NCC* ncc, Value* other);
	void mul(NCC* ncc, Value* other);
	void div(NCC* ncc, Value* other);
};
