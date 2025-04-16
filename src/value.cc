#include <backend/backend.hpp>
#include <hcc.hpp>
#include <value.hpp>

using namespace hcc;

Value::Value(std::string reg_name) {
	this->stack_align = 0;
	this->reg_name = reg_name;
}

Value::Value(uint64_t stack_align) {
	this->stack_align = stack_align;
	this->reg_name = "";
}

Value::Value() {
	this->stack_align = 0;
	this->reg_name = "";
}

bool Value::isRegister() {
	return (reg_name != "");
}

std::string Value::getRegisterName() {
	return reg_name;
}

void Value::add(HCC* hcc, Value* other) {
	if (isRegister()) {
		hcc->assembly_output += hcc->backend->emit_add(reg_name, reg_name, other->reg_name);
	}
}

void Value::sub(HCC* hcc, Value* other) {
	if (isRegister()) {
		hcc->assembly_output += hcc->backend->emit_sub(reg_name, reg_name, other->reg_name);
	}
}

void Value::mul(HCC* hcc, Value* other) {
	if (isRegister()) {
		hcc->assembly_output += hcc->backend->emit_mul(reg_name, reg_name, other->reg_name);
	}
}

void Value::div(HCC* hcc, Value* other) {
	if (isRegister()) {
		hcc->assembly_output += hcc->backend->emit_div(reg_name, reg_name, other->reg_name);
	}
}
