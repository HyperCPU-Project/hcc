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

bool Value::IsRegister() {
  return (reg_name != "");
}

Value* Value::CreateAsRegister(HCC* hcc, uint64_t _value, std::string regName) {
  Value* value = new Value();
  value->reg_name = hcc->backend->EmitMovConst(_value, regName);
  return value;
}

Value* Value::CreateAsCompileTimeValue([[maybe_unused]] HCC* hcc, uint64_t _value) {
  Value* value = new Value();
  value->is_compile_time = true;
  value->compile_time_value = _value;
  return value;
}

Value* Value::CreateAsStackVar(HCC* hcc, TypeMetadata type, bool reserve) {
  Value* value = new Value();

  value->var_stack_align = hcc->current_function.align + type.size;
  value->var_type = type;

  if (reserve)
    hcc->backend->EmitReserveStackSpace(type.size);

  hcc->current_function.align += type.size;

  return value;
}

Value* Value::Use(HCC* hcc) {
  if (!is_compile_time)
    return this;

  Value* value = Value::CreateAsRegister(hcc, compile_time_value);
  return value;
}

Value* Value::DoCondLod(HCC* hcc, std::string load_reg) {
  if (IsRegister() && !is_compile_time)
    return this;
  if (is_compile_time) {
    return Use(hcc);
  }

  auto value = new Value();
  value->reg_name = hcc->backend->EmitLoadFromStack(this->var_stack_align, var_type.size, load_reg);
  return value;
}

void Value::Add(HCC* hcc, Value* other) {
  if (is_compile_time && other->is_compile_time) {
    compile_time_value += other->compile_time_value;
    return;
  }

  Value* LHS = DoCondLod(hcc);
  Value* RHS = other->DoCondLod(hcc);

  hcc->backend->EmitAdd(LHS->reg_name, LHS->reg_name, RHS->reg_name);

  if (!IsRegister()) {
    hcc->backend->EmitStoreToStack(var_stack_align, var_type.size, LHS->reg_name);
  }

  if (LHS != this)
    delete LHS;
  if (RHS != other)
    delete RHS;
}

void Value::Sub(HCC* hcc, Value* other) {
  if (is_compile_time && other->is_compile_time) {
    compile_time_value -= other->compile_time_value;
    return;
  }

  Value* LHS = DoCondLod(hcc);
  Value* RHS = other->DoCondLod(hcc);

  hcc->backend->EmitSub(LHS->reg_name, LHS->reg_name, RHS->reg_name);

  if (!IsRegister()) {
    hcc->backend->EmitStoreToStack(var_stack_align, var_type.size, LHS->reg_name);
  }

  if (LHS != this)
    delete LHS;
  if (RHS != other)
    delete RHS;
}

void Value::Mul(HCC* hcc, Value* other) {
  if (is_compile_time && other->is_compile_time) {
    compile_time_value *= other->compile_time_value;
    return;
  }

  Value* LHS = DoCondLod(hcc);
  Value* RHS = other->DoCondLod(hcc);

  hcc->backend->EmitMul(LHS->reg_name, LHS->reg_name, RHS->reg_name);

  if (!IsRegister()) {
    hcc->backend->EmitStoreToStack(var_stack_align, var_type.size, LHS->reg_name);
  }

  if (LHS != this)
    delete LHS;
  if (RHS != other)
    delete RHS;
}

void Value::Div(HCC* hcc, Value* other) {
  if (is_compile_time && other->is_compile_time) {
    compile_time_value /= other->compile_time_value;
    return;
  }

  Value* LHS = DoCondLod(hcc);
  Value* RHS = other->DoCondLod(hcc);

  hcc->backend->EmitDiv(LHS->reg_name, LHS->reg_name, RHS->reg_name);

  if (!IsRegister()) {
    hcc->backend->EmitStoreToStack(var_stack_align, var_type.size, LHS->reg_name);
  }

  if (LHS != this)
    delete LHS;
  if (RHS != other)
    delete RHS;
}

void Value::SetTo(HCC* hcc, Value* other) {
  if (is_compile_time && other->is_compile_time) {
    compile_time_value = other->compile_time_value;
    return;
  }

  if (!is_compile_time && other->is_compile_time) {
    Value* v = other->Use(hcc);
    if (IsRegister()) {
      hcc->backend->EmitMove(this->reg_name, v->reg_name);
    } else {
      hcc->backend->EmitStoreToStack(var_stack_align, var_type.size, v->reg_name);
    }
    delete v;
  } else if (!IsRegister() && other->IsRegister()) {
    hcc->backend->EmitStoreToStack(var_stack_align, var_type.size, other->reg_name);
  } else if (IsRegister() && other->IsRegister()) {
    hcc->backend->EmitMove(this->reg_name, other->reg_name);
  } else if (!IsRegister() && !other->IsRegister()) {
    Value* LHS = DoCondLod(hcc);
    Value* RHS = other->DoCondLod(hcc);

    hcc->backend->EmitStoreToStack(var_stack_align, var_type.size, LHS->reg_name);

    if (LHS != this)
      delete LHS;
    if (RHS != other)
      delete RHS;
  } else if (IsRegister() && !other->IsRegister()) {
    Value* RHS = other->DoCondLod(hcc);

    hcc->backend->EmitMove(this->reg_name, RHS->reg_name);

    if (RHS != other)
      delete RHS;
  }
}
