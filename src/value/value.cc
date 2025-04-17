#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

Value::Value() {
	reg_name = "";
}

Value::~Value() {
}

bool Value::isRegister() {
	return (reg_name != "");
}

Value* Value::createAsRegister(HCC* hcc, uint64_t _value, std::string reg_name) {
	Value* value = new Value();
	value->reg_name = hcc->backend->emit_mov_const(hcc->getOutFd(), _value, reg_name);
	return value;
}

Value Value::doCondLod(HCC* hcc) {
	if (isRegister())
		return *this;
	// TODO
	return Value();
}

void Value::add(HCC* hcc, Value* other) {
	Value LHS = doCondLod(hcc);
	Value RHS = other->doCondLod(hcc);

	hcc->backend->emit_add(hcc->getOutFd(), LHS.reg_name, LHS.reg_name, RHS.reg_name);
}

void Value::sub(HCC* hcc, Value* other) {
	Value LHS = doCondLod(hcc);
	Value RHS = other->doCondLod(hcc);

	hcc->backend->emit_sub(hcc->getOutFd(), LHS.reg_name, LHS.reg_name, RHS.reg_name);
}

void Value::mul(HCC* hcc, Value* other) {
	Value LHS = doCondLod(hcc);
	Value RHS = other->doCondLod(hcc);

	hcc->backend->emit_mul(hcc->getOutFd(), LHS.reg_name, LHS.reg_name, RHS.reg_name);
}

void Value::div(HCC* hcc, Value* other) {
	Value LHS = doCondLod(hcc);
	Value RHS = other->doCondLod(hcc);

	hcc->backend->emit_div(hcc->getOutFd(), LHS.reg_name, LHS.reg_name, RHS.reg_name);
}