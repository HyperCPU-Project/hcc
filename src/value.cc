#include <backend/backend.hpp>
#include <ncc.hpp>
#include <value.hpp>

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

void Value::add(NCC* ncc, Value* other) {
	if (isRegister()) {
		ncc->assembly_output += ncc->backend->emit_add(reg_name, reg_name, other->reg_name);
	}
}

void Value::sub(NCC* ncc, Value* other) {
	if (isRegister()) {
		ncc->assembly_output += ncc->backend->emit_sub(reg_name, reg_name, other->reg_name);
	}
}

void Value::mul(NCC* ncc, Value* other) {
	if (isRegister()) {
		ncc->assembly_output += ncc->backend->emit_mul(reg_name, reg_name, other->reg_name);
	}
}

void Value::div(NCC* ncc, Value* other) {
	if (isRegister()) {
		ncc->assembly_output += ncc->backend->emit_div(reg_name, reg_name, other->reg_name);
	}
}
