#include <hcc.hpp>
#include <memory>
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

Value* Value::createAsStackVar(HCC* hcc, TypeMetadata type) {
	Value* value = new Value();

	value->var_stack_align = hcc->current_function.align + type.size;
	value->var_type = type;

	hcc->backend->emit_reserve_stack_space(hcc->getOutFd(), type.size);

	hcc->current_function.align += type.size;

	return value;
}

Value* Value::doCondLod(HCC* hcc) {
	if (isRegister())
		return this;
	auto value = new Value();
	value->reg_name = hcc->backend->emit_load_from_stack(hcc->getOutFd(), this->var_stack_align);
	return value;
}

void Value::add(HCC* hcc, Value* other) {
	Value* LHS = doCondLod(hcc);
	Value* RHS = other->doCondLod(hcc);

	hcc->backend->emit_add(hcc->getOutFd(), LHS->reg_name, LHS->reg_name, RHS->reg_name);

	if (!isRegister()) {
		hcc->backend->emit_store_from_stack(hcc->getOutFd(), var_stack_align, LHS->reg_name);
	}

	if (LHS != this)
		delete LHS;
	if (RHS != other)
		delete RHS;
}

void Value::sub(HCC* hcc, Value* other) {
	Value* LHS = doCondLod(hcc);
	Value* RHS = other->doCondLod(hcc);

	hcc->backend->emit_sub(hcc->getOutFd(), LHS->reg_name, LHS->reg_name, RHS->reg_name);

	if (!isRegister()) {
		hcc->backend->emit_store_from_stack(hcc->getOutFd(), var_stack_align, LHS->reg_name);
	}

	if (LHS != this)
		delete LHS;
	if (RHS != other)
		delete RHS;
}

void Value::mul(HCC* hcc, Value* other) {
	Value* LHS = doCondLod(hcc);
	Value* RHS = other->doCondLod(hcc);

	hcc->backend->emit_mul(hcc->getOutFd(), LHS->reg_name, LHS->reg_name, RHS->reg_name);

	if (!isRegister()) {
		hcc->backend->emit_store_from_stack(hcc->getOutFd(), var_stack_align, LHS->reg_name);
	}

	if (LHS != this)
		delete LHS;
	if (RHS != other)
		delete RHS;
}

void Value::div(HCC* hcc, Value* other) {
	Value* LHS = doCondLod(hcc);
	Value* RHS = other->doCondLod(hcc);

	hcc->backend->emit_div(hcc->getOutFd(), LHS->reg_name, LHS->reg_name, RHS->reg_name);

	if (!isRegister()) {
		hcc->backend->emit_store_from_stack(hcc->getOutFd(), var_stack_align, LHS->reg_name);
	}

	if (LHS != this)
		delete LHS;
	if (RHS != other)
		delete RHS;
}

void Value::setto(HCC* hcc, Value* other) {
	if (!isRegister() && other->isRegister()) {
		hcc->backend->emit_store_from_stack(hcc->getOutFd(), var_stack_align, other->reg_name);
	} else if (isRegister() && other->isRegister()) {
		hcc->backend->emit_move(hcc->getOutFd(), this->reg_name, other->reg_name);
	} else if (!isRegister() && !other->isRegister()) {
		Value* LHS = doCondLod(hcc);
		Value* RHS = other->doCondLod(hcc);

		hcc->backend->emit_store_from_stack(hcc->getOutFd(), var_stack_align, LHS->reg_name);

		if (LHS != this)
			delete LHS;
		if (RHS != other)
			delete RHS;
	} else if (isRegister() && !other->isRegister()) {
		Value* RHS = other->doCondLod(hcc);

		hcc->backend->emit_move(hcc->getOutFd(), this->reg_name, RHS->reg_name);

		if (RHS != other)
			delete RHS;
	}
}
