#include <backend/backend.hpp>
#include <hcc.hpp>
#include <ir/ir.hpp>

using namespace hcc;

IR::IR() = default;

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

bool IR::compile(HCC* hcc) {
  IrOpcode current_funcdef_op;
  for (;;) {
    IrOpcode op = Next();
    if (op.type == IrOpcode::IR_END)
      break;

    switch (op.type) {
    case IrOpcode::IR_NULL:
      hcc_compile_error = "IR_NULL opcode encountered";
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
    case IrOpcode::IR_CCTV: {
      hcc->backend->EmitPushImm(op.cctv.value);
    } break;
    case IrOpcode::IR_RET: {
      if (op.ret.has_value) {
        hcc->backend->EmitPop(hcc->backend->abi.return_register);
      }

      if (current_funcdef_op.funcdef.need_stack)
        hcc->backend->EmitFunctionEpilogue();
      else
        hcc->backend->EmitSingleRet();
    } break;
    case IrOpcode::IR_ALLOCA: {
      size_t offset;
      hcc->current_function.align += op.alloca.md.size;
      offset = hcc->current_function.align;
      hcc->current_function.variables[op.alloca.name] = VariableMetadata(offset, op.alloca.md);
    } break;
    case IrOpcode::IR_ADD: {
      hcc->backend->EmitPop(hcc->backend->abi.registers[1]);
      hcc->backend->EmitPop(hcc->backend->abi.registers[0]);
      hcc->backend->EmitAdd(hcc->backend->abi.registers[0], hcc->backend->abi.registers[0], hcc->backend->abi.registers[1]);
      hcc->backend->EmitPush(hcc->backend->abi.registers[0]);
    } break;
    case IrOpcode::IR_SUB: {
      hcc->backend->EmitPop(hcc->backend->abi.registers[1]);
      hcc->backend->EmitPop(hcc->backend->abi.registers[0]);
      hcc->backend->EmitSub(hcc->backend->abi.registers[0], hcc->backend->abi.registers[0], hcc->backend->abi.registers[1]);
      hcc->backend->EmitPush(hcc->backend->abi.registers[0]);
    } break;
    case IrOpcode::IR_MUL: {
      hcc->backend->EmitPop(hcc->backend->abi.registers[1]);
      hcc->backend->EmitPop(hcc->backend->abi.registers[0]);
      hcc->backend->EmitMul(hcc->backend->abi.registers[0], hcc->backend->abi.registers[0], hcc->backend->abi.registers[1]);
      hcc->backend->EmitPush(hcc->backend->abi.registers[0]);
    } break;
    case IrOpcode::IR_DIV: {
      hcc->backend->EmitPop(hcc->backend->abi.registers[1]);
      hcc->backend->EmitPop(hcc->backend->abi.registers[0]);
      hcc->backend->EmitDiv(hcc->backend->abi.registers[0], hcc->backend->abi.registers[0], hcc->backend->abi.registers[1]);
      hcc->backend->EmitPush(hcc->backend->abi.registers[0]);
    } break;
    case IrOpcode::IR_ASSIGN: {
      if (!hcc->current_function.variables.contains(op.assign.name)) {
        hcc_compile_error = fmt::sprintf("undefined variable %s", op.assign.name);
        return false;
      }

      auto& var = hcc->current_function.variables[op.varref.name];
      size_t align = var.offset;
      std::string reg = hcc->backend->abi.registers[1];

      hcc->backend->EmitPop(reg);
      hcc->backend->EmitStoreToStack(align, var.type.size, reg);
    } break;
    case IrOpcode::IR_ASM:
      // TODO: Add a new call in the Backend class for this since emitcalls don't allow doing this
      // hcc->backend->output += op.asm_.code + "\n";
      break;
    case IrOpcode::IR_VARREF: {
      if (!hcc->current_function.variables.contains(op.varref.name)) {
        hcc_compile_error = fmt::sprintf("undefined variable %s", op.varref.name);
        return false;
      }

      auto& var = hcc->current_function.variables[op.varref.name];
      std::string reg = hcc->backend->abi.registers[0];
      hcc->backend->EmitLoadFromStack(var.offset, var.type.size, reg);
      hcc->backend->EmitPush(reg);
    } break;
    case IrOpcode::IR_ADDROF: {
      // todo
    } break;
    case IrOpcode::IR_CALL: {
      hcc->backend->EmitCall(op.call.name);
      hcc->backend->EmitPush(hcc->backend->abi.return_register);
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
