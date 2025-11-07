#include <backend/backend.hpp>
#include <hcc.hpp>
#include <ir/ir.hpp>
#include <value/value.hpp>

using namespace hcc;

IR::IR() = default;

void IR::addLine() {
  IrOpcode op;
  op.type = IrOpcode::IR_LINE;
  ir.push_back(op);
}

void IR::addReset() {
  IrOpcode op;
  op.type = IrOpcode::IR_RESET;
  ir.push_back(op);
}

void IR::add(IrOpcode op) {
  ir.push_back(op);
}

IrOpcode IR::next() {
  if (index >= ir.size()) {
    IrOpcode op;
    op.type = IrOpcode::IR_END;
    return op;
  }
  return ir[index++];
}

IrOpcode IR::peek(unsigned long count) {
  unsigned long indexOld = index;
  IrOpcode op;

  op.type = IrOpcode::IR_NULL;
  for (unsigned long i = 0; i < count; i++) {
    op = next();
  }

  index = indexOld;

  return op;
}

bool IR::opcodeAffectsStack(IrOpcode op) {
  switch (op.type) {
  case IrOpcode::IR_ALLOCA:
    return true;
  case IrOpcode::IR_CSV:
    return true;
  default:
    return false;
  }
}

void IR::performStaticOptimizations(HCC* hcc) {
  {
    IrOpcode op;
    op.type = IrOpcode::IR_END;
    add(op);
  }

  if (hcc->optimizations.hasFlag(HCC::OPT_CONSTANT_PROPAGATION)) {
    optimizeConstantPropagation(hcc);
  }
  if (hcc->optimizations.hasFlag(HCC::OPT_DCE)) {
    optimizeDceUnused(hcc);
  }
  if (hcc->optimizations.hasFlag(HCC::OPT_FP_OMISSION)) {
    optimizeStackSetup(hcc);
  }
  if (hcc->optimizations.hasFlag(HCC::OPT_STACK_RESERVE)) {
    optimizeStackReserve(hcc);
  }
}

bool IR::compile(HCC* hcc) {
  /*for (IrOpcode& op : ir) {
    fmt::println("{}", (int)op.type);
  }*/
  IrOpcode currentFuncdefOp;
  for (;;) {
    IrOpcode op = next();
    if (op.type == IrOpcode::IR_END)
      break;

    switch (op.type) {
    case IrOpcode::IR_NULL:
      hccCompileError = "IR_NULL opcode encountered";
      return false;
      break;
    case IrOpcode::IR_FUNCDEF:
      if (peek().type == IrOpcode::IR_RET && hcc->optimizations.hasFlag(HCC::OPT_FUNCTION_BODY_ELIMINATION)) { // function with no body that instantly returns
        hcc->backend->emitLabel(op.funcdef.name);
        hcc->backend->emitSingleRet();
        next();
      } else {
        if (op.funcdef.needStack) {
          hcc->backend->resetRegIndex();
          hcc->backend->emitFunctionPrologue(op.funcdef.name);
        } else {
          hcc->backend->emitLabel(op.funcdef.name);
        }
        currentFuncdefOp = op;
      }
      break;
    case IrOpcode::IR_CREG: {
      auto value = std::unique_ptr<Value>(Value::createAsRegister(hcc, op.creg.value, op.creg.regName));
      hcc->values.push(std::move(value));
    } break;
    case IrOpcode::IR_CCTV: {
      auto value = std::unique_ptr<Value>(Value::createAsCompileTimeValue(hcc, op.cctv.value));
      hcc->values.push(std::move(value));
    } break;
    case IrOpcode::IR_CSV: {
      auto value = std::unique_ptr<Value>(Value::createAsStackVar(hcc, op.csv.md));
      hcc->values.push(std::move(value));
    } break;
    case IrOpcode::IR_RET: {
      if (!hcc->values.empty()) {
        auto value_raw = std::move(hcc->values.top());
        hcc->values.pop();

        auto value = value_raw->use(hcc);

        if (value->regName != hcc->backend->abi.returnRegister) {
          hcc->backend->emitMove(hcc->backend->abi.returnRegister, value->regName);
        }

        if (value != value_raw.get())
          delete value;
      }

      if (currentFuncdefOp.funcdef.needStack)
        hcc->backend->emitFunctionEpilogue();
      else
        hcc->backend->emitSingleRet();

      while (!hcc->values.empty())
        hcc->values.pop();
    } break;
    case IrOpcode::IR_ALLOCA: {
      std::unique_ptr<Value> value(Value::createAsStackVar(hcc, op.alloca.md, false));
      hcc->currentFunction.variables[op.alloca.name] = std::move(value);
    } break;
    case IrOpcode::IR_ADD: {
      auto RHS = std::move(hcc->values.top());
      hcc->values.pop();
      auto LHS = std::move(hcc->values.top());
      hcc->values.pop();

      LHS->add(hcc, RHS.get());

      hcc->values.push(std::move(LHS));
    } break;
    case IrOpcode::IR_SUB: {
      auto RHS = std::move(hcc->values.top());
      hcc->values.pop();
      auto LHS = std::move(hcc->values.top());
      hcc->values.pop();

      LHS->sub(hcc, RHS.get());

      hcc->values.push(std::move(LHS));
    } break;
    case IrOpcode::IR_MUL: {
      auto RHS = std::move(hcc->values.top());
      hcc->values.pop();
      auto LHS = std::move(hcc->values.top());
      hcc->values.pop();

      LHS->mul(hcc, RHS.get());

      hcc->values.push(std::move(LHS));
    } break;
    case IrOpcode::IR_DIV: {
      auto RHS = std::move(hcc->values.top());
      hcc->values.pop();
      auto LHS = std::move(hcc->values.top());
      hcc->values.pop();

      LHS->div(hcc, RHS.get());

      hcc->values.push(std::move(LHS));
    } break;
    case IrOpcode::IR_ASSIGN: {
      if (!hcc->currentFunction.variables.contains(op.assign.name)) {
        hccCompileError = fmt::sprintf("undefined variable %s", op.assign.name);
        return false;
      }

      auto& expr_var = hcc->currentFunction.variables[op.assign.name];

      auto expr_value = std::move(hcc->values.top());
      hcc->values.pop();

      expr_var->setto(hcc, expr_value.get());
    } break;
    case IrOpcode::IR_ASM:
      hcc->backend->output += op.asm_.code + "\n";
      break;
    case IrOpcode::IR_VARREF: {
      if (!hcc->currentFunction.variables.contains(op.varref.name)) {
        hccCompileError = fmt::sprintf("undefined variable %s", op.varref.name);
        return false;
      }

      std::unique_ptr<Value> out(hcc->currentFunction.variables[op.varref.name]->doCondLod(hcc));

      hcc->values.push(std::move(out));
    } break;
    case IrOpcode::IR_ADDROF: {
      if (!hcc->currentFunction.variables.contains(op.addrof.name)) {
        hccCompileError = fmt::sprintf("undefined variable %s", op.addrof.name);
        return false;
      }

      auto out = std::unique_ptr<Value>(new Value());
      out->regName = hcc->backend->emitLoadaddrFromStack(hcc->currentFunction.variables[op.addrof.name]->varStackAlign);

      hcc->values.push(std::move(out));
    } break;
    case IrOpcode::IR_CALL: {
      hcc->backend->emitCall(op.call.name);
      auto value = std::make_unique<Value>();
      value->regName = hcc->backend->abi.returnRegister;

      hcc->values.push(std::move(value));
    } break;
    case IrOpcode::IR_LINE:
      break;
    case IrOpcode::IR_RESET:
      hcc->backend->resetRegIndex();
      break;
    case IrOpcode::IR_RESERVE:
      if (op.reserve.bytes > 0)
        hcc->backend->emitReserveStackSpae(op.reserve.bytes);
      break;
    default:
      break;
    }
  }
  return true;
}

bool IR::resultsInError(HCC* hcc) {
  index = 0;
  bool result = !compile(hcc);
  hcc->backend->output.clear();
  index = 0;
  return result;
}
