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

	void add(NCC* ncc, Value* other);
	void mul(NCC* ncc, Value* other);
};
