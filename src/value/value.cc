#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

Value::Value() {
	reg_name = "";
	is_compile_time = false;
	compile_time_value = 0;
}

Value::~Value() {
}

bool Value::isRegister() {
	return (reg_name != "");
}

Value* Value::createAsRegister(HCC* hcc, uint64_t _value, std::string reg_name) {
	Value* value = new Value();
	value->reg_name = hcc->backend->emit_mov_const(_value, reg_name);
	return value;
}

Value* Value::createAsCompileTimeValue([[maybe_unused]] HCC* hcc, uint64_t _value) {
	Value* value = new Value();
	value->is_compile_time = true;
	value->compile_time_value = _value;
	return value;
}

Value* Value::createAsStackVar(HCC* hcc, TypeMetadata type, bool reserve) {
	Value* value = new Value();

	value->var_stack_align = hcc->current_function.align + type.size;
	value->var_type = type;

	if (reserve)
		hcc->backend->emit_reserve_stack_space(type.size);

	hcc->current_function.align += type.size;

	return value;
}

Value* Value::use(HCC* hcc) {
	if (!is_compile_time)
		return this;

	Value* value = Value::createAsRegister(hcc, compile_time_value);
	return value;
}

Value* Value::doCondLod(HCC* hcc, std::string load_reg) {
	if (isRegister() && !is_compile_time)
		return this;
	if (is_compile_time) {
		return use(hcc);
	}

	auto value = new Value();
	value->reg_name = hcc->backend->emit_load_from_stack(this->var_stack_align, var_type.size, load_reg);
	return value;
}

void Value::add(HCC* hcc, Value* other) {
	if (is_compile_time && other->is_compile_time) {
		compile_time_value += other->compile_time_value;
		return;
	}

	Value* LHS = doCondLod(hcc);
	Value* RHS = other->doCondLod(hcc);

	hcc->backend->emit_add(LHS->reg_name, LHS->reg_name, RHS->reg_name);

	if (!isRegister()) {
		hcc->backend->emit_store_from_stack(var_stack_align, var_type.size, LHS->reg_name);
	}

	if (LHS != this)
		delete LHS;
	if (RHS != other)
		delete RHS;
}

void Value::sub(HCC* hcc, Value* other) {
	if (is_compile_time && other->is_compile_time) {
		compile_time_value -= other->compile_time_value;
		return;
	}

	Value* LHS = doCondLod(hcc);
	Value* RHS = other->doCondLod(hcc);

	hcc->backend->emit_sub(LHS->reg_name, LHS->reg_name, RHS->reg_name);

	if (!isRegister()) {
		hcc->backend->emit_store_from_stack(var_stack_align, var_type.size, LHS->reg_name);
	}

	if (LHS != this)
		delete LHS;
	if (RHS != other)
		delete RHS;
}

void Value::mul(HCC* hcc, Value* other) {
	if (is_compile_time && other->is_compile_time) {
		compile_time_value *= other->compile_time_value;
		return;
	}

	Value* LHS = doCondLod(hcc);
	Value* RHS = other->doCondLod(hcc);

	hcc->backend->emit_mul(LHS->reg_name, LHS->reg_name, RHS->reg_name);

	if (!isRegister()) {
		hcc->backend->emit_store_from_stack(var_stack_align, var_type.size, LHS->reg_name);
	}

	if (LHS != this)
		delete LHS;
	if (RHS != other)
		delete RHS;
}

void Value::div(HCC* hcc, Value* other) {
	if (is_compile_time && other->is_compile_time) {
		compile_time_value /= other->compile_time_value;
		return;
	}

	Value* LHS = doCondLod(hcc);
	Value* RHS = other->doCondLod(hcc);

	hcc->backend->emit_div(LHS->reg_name, LHS->reg_name, RHS->reg_name);

	if (!isRegister()) {
		hcc->backend->emit_store_from_stack(var_stack_align, var_type.size, LHS->reg_name);
	}

	if (LHS != this)
		delete LHS;
	if (RHS != other)
		delete RHS;
}

void Value::setto(HCC* hcc, Value* other) {
	if (is_compile_time && other->is_compile_time) {
		compile_time_value = other->compile_time_value;
		return;
	}

	if (!is_compile_time && other->is_compile_time) {
		Value* v = other->use(hcc);
		if (isRegister()) {
			hcc->backend->emit_move(this->reg_name, v->reg_name);
		} else {
			hcc->backend->emit_store_from_stack(var_stack_align, var_type.size, v->reg_name);
		}
		delete v;
	} else if (!isRegister() && other->isRegister()) {
		hcc->backend->emit_store_from_stack(var_stack_align, var_type.size, other->reg_name);
	} else if (isRegister() && other->isRegister()) {
		hcc->backend->emit_move(this->reg_name, other->reg_name);
	} else if (!isRegister() && !other->isRegister()) {
		Value* LHS = doCondLod(hcc);
		Value* RHS = other->doCondLod(hcc);

		hcc->backend->emit_store_from_stack(var_stack_align, var_type.size, LHS->reg_name);

		if (LHS != this)
			delete LHS;
		if (RHS != other)
			delete RHS;
	} else if (isRegister() && !other->isRegister()) {
		Value* RHS = other->doCondLod(hcc);

		hcc->backend->emit_move(this->reg_name, RHS->reg_name);

		if (RHS != other)
			delete RHS;
	}
}
