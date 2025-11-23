#include "value/value_stack_var.hpp"
#include <hcc.hpp>
#include <memory>
#include <value/value.hpp>

using namespace hcc;

Value::Value()
    : value(0ULL) {
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

std::shared_ptr<Value> Value::CreateAsRegister(HCC* hcc, uint64_t _value, std::string regName) {
  auto value = std::make_shared<Value>();
  value->value = hcc->backend->EmitMovConst(_value, regName);
  return value;
}

std::shared_ptr<Value> Value::CreateAsCompileTimeValue([[maybe_unused]] HCC* hcc, uint64_t _value) {
  auto value = std::make_shared<Value>();
  value->value = _value;
  return value;
}

std::shared_ptr<Value> Value::CreateAsStackVar(HCC* hcc, TypeMetadata type, bool reserve) {
  auto value = std::make_shared<Value>();

  ValueStackVar var{};
  var.stack_align = hcc->current_function.align + type.size;
  var.type = type;
  value->value = ValueStackVar();

  if (reserve)
    hcc->backend->EmitReserveStackSpace(type.size);

  hcc->current_function.align += type.size;

  return value;
}

std::shared_ptr<Value> Value::Use(HCC* hcc) {
  if (!IsCompileTime())
    return std::make_shared<Value>(*this);

  auto value = Value::CreateAsRegister(hcc, std::get<uint64_t>(this->value));
  return value;
}

std::shared_ptr<Value> Value::DoCondLod(HCC* hcc, std::string load_reg) {
  if (IsStackVar()) {
    auto var = std::get<ValueStackVar>(this->value);

    auto value = std::make_shared<Value>();
    value->value = hcc->backend->EmitLoadFromStack(var.stack_align, var.type.size, load_reg);
    return value;
  } else if (IsCompileTime()) {
    return std::make_shared<Value>(*this);
  } else if (IsRegister()) {
    return Use(hcc);
  }
  std::abort();
}

void Value::Add(HCC* hcc, std::shared_ptr<Value> other) {
  if (IsCompileTime() && other->IsCompileTime()) {
    std::get<uint64_t>(value) += std::get<uint64_t>(other->value);
    return;
  }

  auto LHS = DoCondLod(hcc);
  auto RHS = other->DoCondLod(hcc);
  std::string LHS_reg = std::get<std::string>(LHS->value);
  std::string RHS_reg = std::get<std::string>(RHS->value);

  hcc->backend->EmitAdd(LHS_reg, LHS_reg, RHS_reg);

  if (IsStackVar()) {
    ValueStackVar var = std::get<ValueStackVar>(this->value);
    hcc->backend->EmitStoreToStack(var.stack_align, var.type.size, LHS_reg);
  }
}

void Value::Sub(HCC* hcc, std::shared_ptr<Value> other) {
  if (IsCompileTime() && other->IsCompileTime()) {
    std::get<uint64_t>(this->value) -= std::get<uint64_t>(other->value);
    return;
  }

  auto LHS = DoCondLod(hcc);
  auto RHS = other->DoCondLod(hcc);
  std::string LHS_reg = std::get<std::string>(LHS->value);
  std::string RHS_reg = std::get<std::string>(RHS->value);

  hcc->backend->EmitSub(LHS_reg, LHS_reg, RHS_reg);

  if (IsStackVar()) {
    ValueStackVar var = std::get<ValueStackVar>(this->value);
    hcc->backend->EmitStoreToStack(var.stack_align, var.type.size, LHS_reg);
  }
}

void Value::Mul(HCC* hcc, std::shared_ptr<Value> other) {
  if (IsCompileTime() && other->IsCompileTime()) {
    std::get<uint64_t>(this->value) *= std::get<uint64_t>(other->value);
    return;
  }

  std::shared_ptr<Value> LHS = DoCondLod(hcc);
  std::shared_ptr<Value> RHS = other->DoCondLod(hcc);
  std::string LHS_reg = std::get<std::string>(LHS->value);
  std::string RHS_reg = std::get<std::string>(RHS->value);

  hcc->backend->EmitMul(LHS_reg, LHS_reg, RHS_reg);

  if (IsStackVar()) {
    ValueStackVar var = std::get<ValueStackVar>(this->value);
    hcc->backend->EmitStoreToStack(var.stack_align, var.type.size, LHS_reg);
  }
}

void Value::Div(HCC* hcc, std::shared_ptr<Value> other) {
  if (IsCompileTime() && other->IsCompileTime()) {
    std::get<uint64_t>(this->value) /= std::get<uint64_t>(other->value);
    return;
  }

  std::shared_ptr<Value> LHS = DoCondLod(hcc);
  std::shared_ptr<Value> RHS = other->DoCondLod(hcc);
  std::string LHS_reg = std::get<std::string>(LHS->value);
  std::string RHS_reg = std::get<std::string>(RHS->value);

  hcc->backend->EmitDiv(LHS_reg, LHS_reg, RHS_reg);

  if (IsStackVar()) {
    ValueStackVar var = std::get<ValueStackVar>(this->value);
    hcc->backend->EmitStoreToStack(var.stack_align, var.type.size, LHS_reg);
  }
}

void Value::SetTo(HCC* hcc, std::shared_ptr<Value> other) {
  if (IsCompileTime() && other->IsCompileTime()) {
    std::get<uint64_t>(this->value) = std::get<uint64_t>(other->value);
    return;
  }

  if (!IsCompileTime() && other->IsCompileTime()) {
    auto v = other->Use(hcc);
    if (IsRegister()) {
      hcc->backend->EmitMove(std::get<std::string>(this->value), std::get<std::string>(v->value));
    } else {
      ValueStackVar var = std::get<ValueStackVar>(this->value);
      hcc->backend->EmitStoreToStack(var.stack_align, var.type.size, std::get<std::string>(v->value));
    }
  } else if (!IsRegister() && other->IsRegister()) {
    ValueStackVar var = std::get<ValueStackVar>(this->value);
    hcc->backend->EmitStoreToStack(var.stack_align, var.type.size, std::get<std::string>(other->value));
  } else if (IsRegister() && other->IsRegister()) {
    hcc->backend->EmitMove(std::get<std::string>(this->value), std::get<std::string>(other->value));
  } else if (!IsRegister() && !other->IsRegister()) {
    auto LHS = DoCondLod(hcc);
    auto RHS = other->DoCondLod(hcc);

    ValueStackVar var = std::get<ValueStackVar>(this->value);
    hcc->backend->EmitStoreToStack(var.stack_align, var.type.size, std::get<std::string>(LHS->value));
  } else if (IsRegister() && !other->IsRegister()) {
    auto RHS = other->DoCondLod(hcc);

    hcc->backend->EmitMove(std::get<std::string>(this->value), std::get<std::string>(RHS->value));
  }
}
