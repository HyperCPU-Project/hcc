#include <backend/backend.hpp>
#include <hcc.hpp>
#include <ir/ir.hpp>

using namespace hcc;

IR::IR() = default;

void IR::add_line() {
  IrOpcode op;
  op.type = IrOpcode::IR_LINE;
  ir.push_back(op);
}

void IR::add_reset() {
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
  unsigned long index_old = index;
  IrOpcode op;

  op.type = IrOpcode::IR_NULL;
  for (unsigned long i = 0; i < count; i++) {
    op = next();
  }

  index = index_old;

  return op;
}

bool IR::opcode_affects_stack(IrOpcode op) {
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

  for (IrOpcode& op : ir) {
    fmt::println("{}", (int)op.type);
  }

  if (hcc->optimizations.HasFlag(HCC::OPT_CONSTANT_PROPAGATION)) {
    optimize_constant_propagation(hcc);
  }
  if (hcc->optimizations.HasFlag(HCC::OPT_DCE)) {
    optimize_dce_unused(hcc);
  }
  if (hcc->optimizations.HasFlag(HCC::OPT_FP_OMISSION)) {
    optimize_stack_setup(hcc);
  }
  if (hcc->optimizations.HasFlag(HCC::OPT_STACK_RESERVE)) {
    optimize_stack_reserve(hcc);
  }
}

bool IR::compile(HCC* hcc) {
  IrOpcode currentFuncdefOp;
  for (;;) {
    IrOpcode op = next();
    if (op.type == IrOpcode::IR_END)
      break;

    switch (op.type) {
    case IrOpcode::IR_NULL:
      hcc_compile_error = "IR_NULL opcode encountered";
      return false;
      break;
    case IrOpcode::IR_FUNCDEF:
      if (peek().type == IrOpcode::IR_RET && hcc->optimizations.HasFlag(HCC::OPT_FUNCTION_BODY_ELIMINATION)) { // function with no body that instantly returns
        hcc->backend->emit_label(op.funcdef.name);
        hcc->backend->emit_single_ret();
        next();
      } else {
        if (op.funcdef.need_stack) {
          hcc->backend->reset_reg_index();
          hcc->backend->emit_function_prologue(op.funcdef.name);
        } else {
          hcc->backend->emit_label(op.funcdef.name);
        }
        currentFuncdefOp = op;
      }
      break;
    case IrOpcode::IR_CCTV: {
      hcc->backend->emit_push_imm(op.cctv.value);
    } break;
    case IrOpcode::IR_RET: {
      if (op.ret.has_value) {
        hcc->backend->emit_pop(hcc->backend->abi.return_register);
      }

      if (currentFuncdefOp.funcdef.need_stack)
        hcc->backend->emit_function_epilogue();
      else
        hcc->backend->emit_single_ret();
    } break;
    case IrOpcode::IR_ALLOCA: {
      /* std::unique_ptr<Value> value(Value::createAsStackVar(hcc, op.alloca.md, false));
       hcc->current_function.variables[op.alloca.name] = std::move(value); */
      size_t align;
      align = hcc->current_function.align + op.alloca.md.size;
      hcc->current_function.variables[op.alloca.name] = align;

      // hcc->backend->emit_reserve_stack_space(op.alloca.md.size);
    } break;
    case IrOpcode::IR_ADD: {
      hcc->backend->emit_pop(hcc->backend->abi.registers[1]);
      hcc->backend->emit_pop(hcc->backend->abi.registers[0]);
      hcc->backend->emit_add(hcc->backend->abi.registers[0], hcc->backend->abi.registers[0], hcc->backend->abi.registers[1]);
      hcc->backend->emit_push(hcc->backend->abi.registers[0]);
    } break;
    case IrOpcode::IR_SUB: {
      hcc->backend->emit_pop(hcc->backend->abi.registers[1]);
      hcc->backend->emit_pop(hcc->backend->abi.registers[0]);
      hcc->backend->emit_sub(hcc->backend->abi.registers[0], hcc->backend->abi.registers[0], hcc->backend->abi.registers[1]);
      hcc->backend->emit_push(hcc->backend->abi.registers[0]);
    } break;
    case IrOpcode::IR_MUL: {
      hcc->backend->emit_pop(hcc->backend->abi.registers[1]);
      hcc->backend->emit_pop(hcc->backend->abi.registers[0]);
      hcc->backend->emit_mul(hcc->backend->abi.registers[0], hcc->backend->abi.registers[0], hcc->backend->abi.registers[1]);
      hcc->backend->emit_push(hcc->backend->abi.registers[0]);
    } break;
    case IrOpcode::IR_DIV: {
      hcc->backend->emit_pop(hcc->backend->abi.registers[1]);
      hcc->backend->emit_pop(hcc->backend->abi.registers[0]);
      hcc->backend->emit_div(hcc->backend->abi.registers[0], hcc->backend->abi.registers[0], hcc->backend->abi.registers[1]);
      hcc->backend->emit_push(hcc->backend->abi.registers[0]);
    } break;
    case IrOpcode::IR_ASSIGN: {
      if (!hcc->current_function.variables.contains(op.assign.name)) {
        hcc_compile_error = fmt::sprintf("undefined variable %s", op.assign.name);
        return false;
      }

      size_t align = hcc->current_function.variables[op.assign.name];
      std::string reg = hcc->backend->abi.registers[1];

      hcc->backend->emit_pop(reg);
      hcc->backend->emit_store_from_stack(align, 4, reg);
      /*

      auto expr_value = std::move(hcc->values.top());
      hcc->values.pop();

      expr_var->setto(hcc, expr_value.get());
      */
    } break;
    case IrOpcode::IR_ASM:
      hcc->backend->output += op.asm_.code + "\n";
      break;
    case IrOpcode::IR_VARREF: {
      /*
if (!hcc->current_function.variables.contains(op.varref.name)) {
hcc_compile_error = fmt::sprintf("undefined variable %s", op.varref.name);
return false;
}

std::unique_ptr<Value> out(hcc->current_function.variables[op.varref.name]->doCondLod(hcc));

hcc->values.push(std::move(out));*/
    } break;
    case IrOpcode::IR_ADDROF: {
      /*
if (!hcc->current_function.variables.contains(op.addrof.name)) {
hcc_compile_error = fmt::sprintf("undefined variable %s", op.addrof.name);
return false;
}

auto out = std::unique_ptr<Value>(new Value());
out->reg_name = hcc->backend->emit_loadaddr_from_stack(hcc->current_function.variables[op.addrof.name]->var_stack_align);

hcc->values.push(std::move(out));
*/
    } break;
    case IrOpcode::IR_CALL: {
      hcc->backend->emit_call(op.call.name);
      hcc->backend->emit_push(hcc->backend->abi.return_register);
    } break;
    case IrOpcode::IR_LINE:
      break;
    case IrOpcode::IR_RESET:
      hcc->backend->reset_reg_index();
      break;
    case IrOpcode::IR_RESERVE:
      if (op.reserve.bytes > 0)
        hcc->backend->emit_reserve_stack_space(op.reserve.bytes);
      break;
    default:
      break;
    }
  }
  return true;
}

bool IR::results_in_error(HCC* hcc) {
  index = 0;
  bool result = !compile(hcc);
  hcc->backend->output.clear();
  index = 0;
  return result;
}
