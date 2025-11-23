#include <backend/backend.hpp>
#include <hcc.hpp>
#include <ir/ir.hpp>
#include <value/value.hpp>

using namespace hcc;

IR::IR() : index(0) {};

void IR::AddLine() {
  IrOpcode op;
  op.type = IrOpcode::IR_LINE;
  ir.push_back(op);
}

void IR::AddReset() {
  IrOpcode op;
  op.type = IrOpcode::IR_RESET;
  ir.push_back(op);
}

void IR::Add(IrOpcode op) {
  ir.push_back(op);
}

IrOpcode IR::Next() {
  if (index >= ir.size()) {
    IrOpcode op;
    op.type = IrOpcode::IR_END;
    return op;
  }
  return ir[index++];
}

IrOpcode IR::Peek(unsigned long count) {
  unsigned long indexOld = index;
  IrOpcode op;

  op.type = IrOpcode::IR_NULL;
  for (unsigned long i = 0; i < count; i++) {
    op = Next();
  }

  index = indexOld;

  return op;
}

bool IR::OpcodeAffectsStack(IrOpcode op) {
  switch (op.type) {
  case IrOpcode::IR_ALLOCA:
    return true;
  case IrOpcode::IR_CSV:
    return true;
  default:
    return false;
  }
}

void IR::PerformStaticOptimizations(HCC* hcc) {
  {
    IrOpcode op;
    op.type = IrOpcode::IR_END;
    Add(op);
  }

  if (hcc->optimizations.HasFlag(Optimization::OPT_CONSTANT_PROPAGATION)) {
    OptimizeConstantPropagation(hcc);
  }
  if (hcc->optimizations.HasFlag(Optimization::OPT_DCE)) {
    OptimizeDceUnused(hcc);
  }
  if (hcc->optimizations.HasFlag(Optimization::OPT_FP_OMISSION)) {
    OptimizeStackSetup(hcc);
  }
  if (hcc->optimizations.HasFlag(Optimization::OPT_STACK_RESERVE)) {
    OptimizeStackReserve(hcc);
  }
}

bool IR::Compile(HCC* hcc) {
  /*for (IrOpcode& op : ir) {
    fmt::println("{}", (int)op.type);
  }*/
  IrOpcode current_funcdef_op;
  for (;;) {
    IrOpcode op = Next();
    if (op.type == IrOpcode::IR_END)
      break;

    switch (op.type) {
    case IrOpcode::IR_NULL:
      hcc->compile_error = "IR_NULL opcode encountered";
      return false;
      break;
    case IrOpcode::IR_FUNCDEF:
      if (Peek().type == IrOpcode::IR_RET && hcc->optimizations.HasFlag(Optimization::OPT_FUNCTION_BODY_ELIMINATION)) { // function with no body that instantly returns
        hcc->backend->EmitLabel(op.funcdef.name);
        hcc->backend->EmitSingleRet();
        Next();
      } else {
        if (op.funcdef.need_stack) {
          hcc->backend->EmitFunctionPrologue(op.funcdef.name);
        } else {
          hcc->backend->EmitLabel(op.funcdef.name);
        }
        current_funcdef_op = op;
      }
      break;
    case IrOpcode::IR_CREG: {
      auto value = Value::CreateAsRegister(hcc, op.creg.value);
      hcc->values.push(std::move(value));
    } break;
    case IrOpcode::IR_CCTV: {
      auto value = Value::CreateAsCompileTimeValue(hcc, op.cctv.value);
      hcc->values.push(std::move(value));
    } break;
    case IrOpcode::IR_CSV: {
      auto value = Value::CreateAsStackVar(hcc, op.csv.md);
      hcc->values.push(std::move(value));
    } break;
    case IrOpcode::IR_RET: {
      if (!hcc->values.empty()) {
        auto value = hcc->values.top()->Use(hcc);
        hcc->values.pop();

        std::string reg = std::get<std::string>(value->value);
        hcc->backend->EmitMove(hcc->backend->abi.return_register, reg);
        hcc->backend->ReleaseRegister(reg);
      }

      if (current_funcdef_op.funcdef.need_stack)
        hcc->backend->EmitFunctionEpilogue();
      else
        hcc->backend->EmitSingleRet();

      while (!hcc->values.empty())
        hcc->values.pop();
    } break;
    case IrOpcode::IR_ALLOCA: {
      auto value = Value::CreateAsStackVar(hcc, op.alloca.md, false);
      hcc->current_function.variables[op.alloca.name] = value;
    } break;
    case IrOpcode::IR_ADD: {
      auto RHS = std::move(hcc->values.top());
      hcc->values.pop();
      auto LHS = std::move(hcc->values.top());
      hcc->values.pop();

      LHS->Add(hcc, RHS);

      hcc->values.push(std::move(LHS));
    } break;
    case IrOpcode::IR_SUB: {
      auto RHS = std::move(hcc->values.top());
      hcc->values.pop();
      auto LHS = std::move(hcc->values.top());
      hcc->values.pop();

      LHS->Sub(hcc, RHS);

      hcc->values.push(std::move(LHS));
    } break;
    case IrOpcode::IR_MUL: {
      auto RHS = std::move(hcc->values.top());
      hcc->values.pop();
      auto LHS = std::move(hcc->values.top());
      hcc->values.pop();

      LHS->Mul(hcc, RHS);

      hcc->values.push(std::move(LHS));
    } break;
    case IrOpcode::IR_DIV: {
      auto RHS = std::move(hcc->values.top());
      hcc->values.pop();
      auto LHS = std::move(hcc->values.top());
      hcc->values.pop();

      LHS->Div(hcc, RHS);

      hcc->values.push(std::move(LHS));
    } break;
    case IrOpcode::IR_ASSIGN: {
      if (!hcc->current_function.variables.contains(op.assign.name)) {
        hcc->compile_error = fmt::sprintf("undefined variable %s", op.assign.name);
        return false;
      }

      auto expr_value = std::move(hcc->values.top());
      hcc->values.pop();

      hcc->current_function.variables[op.assign.name]->SetTo(hcc, expr_value);
    } break;
    case IrOpcode::IR_ASM:
      hcc->backend->output += op.asm_.code + "\n";
      break;
    case IrOpcode::IR_VARREF: {
      if (!hcc->current_function.variables.contains(op.varref.name)) {
        hcc->compile_error = fmt::sprintf("undefined variable %s", op.varref.name);
        return false;
      }

      auto out = hcc->current_function.variables[op.varref.name]->DoCondLod(hcc);

      hcc->values.push(std::move(out));
    } break;
    case IrOpcode::IR_ADDROF: {
      if (!hcc->current_function.variables.contains(op.addrof.name)) {
        hcc->compile_error = fmt::sprintf("undefined variable %s", op.addrof.name);
        return false;
      }

      auto out = std::unique_ptr<Value>(new Value());
      auto var = std::get<ValueStackVar>(hcc->current_function.variables[op.addrof.name]->value);
      out->value = hcc->backend->EmitLoadaddrFromStack(var.stack_align);

      hcc->values.push(std::move(out));
    } break;
    case IrOpcode::IR_CALL: {
      hcc->backend->EmitCall(op.call.name);
      auto value = std::make_unique<Value>();
      value->value = hcc->backend->abi.return_register;

      hcc->values.push(std::move(value));
    } break;
    case IrOpcode::IR_LINE:
      break;
    case IrOpcode::IR_RESET:
      break;
    case IrOpcode::IR_RESERVE:
      if (op.reserve.bytes > 0)
        hcc->backend->EmitReserveStackSpace(op.reserve.bytes);
      break;
    default:
      break;
    }
  }
  return true;
}

bool IR::ResultsInError(HCC* hcc) {
  index = 0;
  bool result = !Compile(hcc);
  hcc->backend->output.clear();
  index = 0;
  return result;
}
