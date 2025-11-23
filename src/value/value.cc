#include "value/value_stack_var.hpp"
#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

Value::Value() {
  value = 0ULL;
}

Value::~Value() {
}

bool Value::IsRegister() {
  return (std::holds_alternative<std::string>(value));
}

bool Value::IsCompileTime() {
  return (std::holds_alternative<uint64_t>(value));
}

bool Value::IsStackVar() {
  return (std::holds_alternative<ValueStackVar>(value));
}

Value* Value::CreateAsRegister(HCC* hcc, uint64_t _value, std::string regName) {
  Value* value = new Value();
  value->value = hcc->backend->EmitMovConst(_value, regName);
  return value;
}

Value* Value::CreateAsCompileTimeValue([[maybe_unused]] HCC* hcc, uint64_t _value) {
  Value* value = new Value();
  value->value = _value;
  return value;
}

Value* Value::CreateAsStackVar(HCC* hcc, TypeMetadata type, bool reserve) {
  Value* value = new Value();

  ValueStackVar var{};
  var.stack_align = hcc->current_function.align + type.size;
  var.type = type;
  value->value = ValueStackVar();

  if (reserve)
    hcc->backend->EmitReserveStackSpace(type.size);

  hcc->current_function.align += type.size;

  return value;
}

Value* Value::Use(HCC* hcc) {
  if (!IsCompileTime())
    return this;

  Value* value = Value::CreateAsRegister(hcc, std::get<uint64_t>(this->value));
  return value;
}

Value* Value::DoCondLod(HCC* hcc, std::string load_reg) {
  if (IsRegister() && !IsCompileTime())
    return this;
  if (!IsCompileTime()) {
    return Use(hcc);
  }

  auto var = std::get<ValueStackVar>(this->value);

  auto value = new Value();
  value->value = hcc->backend->EmitLoadFromStack(var.stack_align, var.type.size, load_reg);
  return value;
}

void Value::Add(HCC* hcc, Value* other) {
  if (IsCompileTime() && other->IsCompileTime()) {
    std::get<uint64_t>(value) += std::get<uint64_t>(other->value);
    return;
  }

  Value* LHS = DoCondLod(hcc);
  Value* RHS = other->DoCondLod(hcc);
  std::string LHS_reg = std::get<std::string>(LHS->value);
  std::string RHS_reg = std::get<std::string>(RHS->value);

  hcc->backend->EmitAdd(LHS_reg, LHS_reg, RHS_reg);

  if (IsStackVar()) {
    ValueStackVar var = std::get<ValueStackVar>(this->value);
    hcc->backend->EmitStoreToStack(var.stack_align, var.type.size, LHS_reg);
  }

  if (LHS != this)
    delete LHS;
  if (RHS != other)
    delete RHS;
}

void Value::Sub(HCC* hcc, Value* other) {
  if (IsCompileTime() && other->IsCompileTime()) {
    std::get<uint64_t>(this->value) -= std::get<uint64_t>(other->value);
    return;
  }

  Value* LHS = DoCondLod(hcc);
  Value* RHS = other->DoCondLod(hcc);
  std::string LHS_reg = std::get<std::string>(LHS->value);
  std::string RHS_reg = std::get<std::string>(RHS->value);

  hcc->backend->EmitSub(LHS_reg, LHS_reg, RHS_reg);

  if (IsStackVar()) {
    ValueStackVar var = std::get<ValueStackVar>(this->value);
    hcc->backend->EmitStoreToStack(var.stack_align, var.type.size, LHS_reg);
  }

  if (LHS != this)
    delete LHS;
  if (RHS != other)
    delete RHS;
}

void Value::Mul(HCC* hcc, Value* other) {
  if (IsCompileTime() && other->IsCompileTime()) {
    std::get<uint64_t>(this->value) *= std::get<uint64_t>(other->value);
    return;
  }

  Value* LHS = DoCondLod(hcc);
  Value* RHS = other->DoCondLod(hcc);
  std::string LHS_reg = std::get<std::string>(LHS->value);
  std::string RHS_reg = std::get<std::string>(RHS->value);

  hcc->backend->EmitMul(LHS_reg, LHS_reg, RHS_reg);

  if (IsStackVar()) {
    ValueStackVar var = std::get<ValueStackVar>(this->value);
    hcc->backend->EmitStoreToStack(var.stack_align, var.type.size, LHS_reg);
  }

  if (LHS != this)
    delete LHS;
  if (RHS != other)
    delete RHS;
}

void Value::Div(HCC* hcc, Value* other) {
  if (IsCompileTime() && other->IsCompileTime()) {
    std::get<uint64_t>(this->value) /= std::get<uint64_t>(other->value);
    return;
  }

  Value* LHS = DoCondLod(hcc);
  Value* RHS = other->DoCondLod(hcc);
  std::string LHS_reg = std::get<std::string>(LHS->value);
  std::string RHS_reg = std::get<std::string>(RHS->value);

  hcc->backend->EmitDiv(LHS_reg, LHS_reg, RHS_reg);

  if (IsStackVar()) {
    ValueStackVar var = std::get<ValueStackVar>(this->value);
    hcc->backend->EmitStoreToStack(var.stack_align, var.type.size, LHS_reg);
  }

  if (LHS != this)
    delete LHS;
  if (RHS != other)
    delete RHS;
}

void Value::SetTo(HCC* hcc, Value* other) {
  if (IsCompileTime() && other->IsCompileTime()) {
    std::get<uint64_t>(this->value) = std::get<uint64_t>(other->value);
    return;
  }

  if (!IsCompileTime() && other->IsCompileTime()) {
    Value* v = other->Use(hcc);
    if (IsRegister()) {
      hcc->backend->EmitMove(std::get<std::string>(this->value), std::get<std::string>(v->value));
    } else {
      ValueStackVar var = std::get<ValueStackVar>(this->value);
      hcc->backend->EmitStoreToStack(var.stack_align, var.type.size, std::get<std::string>(v->value));
    }
    delete v;
  } else if (!IsRegister() && other->IsRegister()) {
    ValueStackVar var = std::get<ValueStackVar>(this->value);
    hcc->backend->EmitStoreToStack(var.stack_align, var.type.size, std::get<std::string>(other->value));
  } else if (IsRegister() && other->IsRegister()) {
    hcc->backend->EmitMove(std::get<std::string>(this->value), std::get<std::string>(other->value));
  } else if (!IsRegister() && !other->IsRegister()) {
    Value* LHS = DoCondLod(hcc);
    Value* RHS = other->DoCondLod(hcc);

    ValueStackVar var = std::get<ValueStackVar>(this->value);
    hcc->backend->EmitStoreToStack(var.stack_align, var.type.size, std::get<std::string>(LHS->value));

    if (LHS != this)
      delete LHS;
    if (RHS != other)
      delete RHS;
  } else if (IsRegister() && !other->IsRegister()) {
    Value* RHS = other->DoCondLod(hcc);

    hcc->backend->EmitMove(std::get<std::string>(this->value), std::get<std::string>(RHS->value));

    if (RHS != other)
      delete RHS;
  }
}
