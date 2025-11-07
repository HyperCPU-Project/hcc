#include <hcc.hpp>
#include <value/value.hpp>

using namespace hcc;

Value::Value() {
  regName = "";
  isCompileTime = false;
  compileTimeValue = 0;
}

Value::~Value() {
}

bool Value::isRegister() {
  return (regName != "");
}

Value* Value::createAsRegister(HCC* hcc, uint64_t _value, std::string regName) {
  Value* value = new Value();
  value->regName = hcc->backend->emitMovConst(_value, regName);
  return value;
}

Value* Value::createAsCompileTimeValue([[maybe_unused]] HCC* hcc, uint64_t _value) {
  Value* value = new Value();
  value->isCompileTime = true;
  value->compileTimeValue = _value;
  return value;
}

Value* Value::createAsStackVar(HCC* hcc, TypeMetadata type, bool reserve) {
  Value* value = new Value();

  value->varStackAlign = hcc->currentFunction.align + type.size;
  value->varType = type;

  if (reserve)
    hcc->backend->emitReserveStackSpae(type.size);

  hcc->currentFunction.align += type.size;

  return value;
}

Value* Value::use(HCC* hcc) {
  if (!isCompileTime)
    return this;

  Value* value = Value::createAsRegister(hcc, compileTimeValue);
  return value;
}

Value* Value::doCondLod(HCC* hcc, std::string load_reg) {
  if (isRegister() && !isCompileTime)
    return this;
  if (isCompileTime) {
    return use(hcc);
  }

  auto value = new Value();
  value->regName = hcc->backend->emitLoadFromStack(this->varStackAlign, varType.size, load_reg);
  return value;
}

void Value::add(HCC* hcc, Value* other) {
  if (isCompileTime && other->isCompileTime) {
    compileTimeValue += other->compileTimeValue;
    return;
  }

  Value* LHS = doCondLod(hcc);
  Value* RHS = other->doCondLod(hcc);

  hcc->backend->emitAdd(LHS->regName, LHS->regName, RHS->regName);

  if (!isRegister()) {
    hcc->backend->emitStoreToStack(varStackAlign, varType.size, LHS->regName);
  }

  if (LHS != this)
    delete LHS;
  if (RHS != other)
    delete RHS;
}

void Value::sub(HCC* hcc, Value* other) {
  if (isCompileTime && other->isCompileTime) {
    compileTimeValue -= other->compileTimeValue;
    return;
  }

  Value* LHS = doCondLod(hcc);
  Value* RHS = other->doCondLod(hcc);

  hcc->backend->emitSub(LHS->regName, LHS->regName, RHS->regName);

  if (!isRegister()) {
    hcc->backend->emitStoreToStack(varStackAlign, varType.size, LHS->regName);
  }

  if (LHS != this)
    delete LHS;
  if (RHS != other)
    delete RHS;
}

void Value::mul(HCC* hcc, Value* other) {
  if (isCompileTime && other->isCompileTime) {
    compileTimeValue *= other->compileTimeValue;
    return;
  }

  Value* LHS = doCondLod(hcc);
  Value* RHS = other->doCondLod(hcc);

  hcc->backend->emitMul(LHS->regName, LHS->regName, RHS->regName);

  if (!isRegister()) {
    hcc->backend->emitStoreToStack(varStackAlign, varType.size, LHS->regName);
  }

  if (LHS != this)
    delete LHS;
  if (RHS != other)
    delete RHS;
}

void Value::div(HCC* hcc, Value* other) {
  if (isCompileTime && other->isCompileTime) {
    compileTimeValue /= other->compileTimeValue;
    return;
  }

  Value* LHS = doCondLod(hcc);
  Value* RHS = other->doCondLod(hcc);

  hcc->backend->emitDiv(LHS->regName, LHS->regName, RHS->regName);

  if (!isRegister()) {
    hcc->backend->emitStoreToStack(varStackAlign, varType.size, LHS->regName);
  }

  if (LHS != this)
    delete LHS;
  if (RHS != other)
    delete RHS;
}

void Value::setto(HCC* hcc, Value* other) {
  if (isCompileTime && other->isCompileTime) {
    compileTimeValue = other->compileTimeValue;
    return;
  }

  if (!isCompileTime && other->isCompileTime) {
    Value* v = other->use(hcc);
    if (isRegister()) {
      hcc->backend->emitMove(this->regName, v->regName);
    } else {
      hcc->backend->emitStoreToStack(varStackAlign, varType.size, v->regName);
    }
    delete v;
  } else if (!isRegister() && other->isRegister()) {
    hcc->backend->emitStoreToStack(varStackAlign, varType.size, other->regName);
  } else if (isRegister() && other->isRegister()) {
    hcc->backend->emitMove(this->regName, other->regName);
  } else if (!isRegister() && !other->isRegister()) {
    Value* LHS = doCondLod(hcc);
    Value* RHS = other->doCondLod(hcc);

    hcc->backend->emitStoreToStack(varStackAlign, varType.size, LHS->regName);

    if (LHS != this)
      delete LHS;
    if (RHS != other)
      delete RHS;
  } else if (isRegister() && !other->isRegister()) {
    Value* RHS = other->doCondLod(hcc);

    hcc->backend->emitMove(this->regName, RHS->regName);

    if (RHS != other)
      delete RHS;
  }
}
