#include <backend/hypercpu/hypercpu_backend.hpp>

using namespace hcc;

std::string HyperCPUBackend::compile_calls() {
  std::string output;
  for (EmitCall ec : emitcalls) {
    switch (ec.type) {
    case Backend::EmitCall::FUNCTION_PROLOGUE:
      if (codegen_comments)
        output += "// emit_function_prologue\n";
      output += fmt::sprintf("%s:\n", ec.function_prologue.name);
      output += fmt::sprintf("push xbp;\nmov xbp, xsp;\n");
      break;
    case Backend::EmitCall::FUNCTION_EPILOGUE:
      if (codegen_comments)
        output += "// emit_function_epilogue\n";
      output += fmt::sprintf("mov xsp, xbp;\npop xbp;\nret;\n");
      break;
    case Backend::EmitCall::MOV_CONST:
      if (codegen_comments)
        output += "// emit_mov_const\n";
      output += fmt::sprintf("mov %s, 0u%ld;\n", ec.mov_const.reg_name, ec.mov_const.value);
      break;
    case Backend::EmitCall::ADD:
      if (codegen_comments)
        output += "// emit_add\n";
      if (ec.add.ROUT != ec.add.RLHS) {
        output += fmt::sprintf("add %s, %s;\nmov %s, %s;\n", ec.add.RLHS, ec.add.RRHS, ec.add.ROUT, ec.add.RLHS);
        break;
      }
      output += fmt::sprintf("add %s, %s;\n", ec.add.RLHS, ec.add.RRHS);
      break;
    case Backend::EmitCall::SUB:
      if (codegen_comments)
        output += "// emit_sub\n";
      if (ec.sub.ROUT != ec.sub.RLHS) {
        output += fmt::sprintf("sub %s, %s;\nmov %s, %s;\n", ec.sub.RLHS, ec.sub.RRHS, ec.sub.ROUT, ec.sub.RLHS);
        break;
      }
      output += fmt::sprintf("sub %s, %s;\n", ec.sub.RLHS, ec.sub.RRHS);
      break;
    case Backend::EmitCall::MUL:
      if (codegen_comments)
        output += "// emit_mul\n";
      if (ec.mul.ROUT != ec.mul.RLHS) {
        output += fmt::sprintf("mul %s, %s;\nmov %s, %s;\n", ec.mul.RLHS, ec.mul.RRHS, ec.mul.ROUT, ec.mul.RLHS);
        break;
      }
      output += fmt::sprintf("mul %s, %s;\n", ec.mul.RLHS, ec.mul.RRHS);
      break;
    case Backend::EmitCall::DIV:
      if (codegen_comments)
        output += "// emit_div\n";
      // RLHS - x0
      // RRHS - x2
      output += fmt::sprintf("push x1;\n", ec.div.RLHS);
      if (ec.div.RRHS != "x2")
        output += fmt::sprintf("mov x2, %s;\n", ec.div.RRHS);
      output += fmt::sprintf("div %s;\n", ec.div.RLHS);
      output += fmt::sprintf("pop x1;\n", ec.div.RLHS);
      if (ec.div.ROUT != ec.div.RLHS) {
        output += fmt::sprintf("mov %s, %s;\n", ec.div.RLHS, ec.div.ROUT);
      }
      break;
    case Backend::EmitCall::MOVE:
      if (ec.move.rdest != ec.move.rsrc) {
        if (codegen_comments)
          output += "// emit_move\n";
        output += fmt::sprintf("mov %s, %s;\n", ec.move.rdest, ec.move.rsrc);
      }
      break;
    case Backend::EmitCall::RESERVE_STACK_SPACE:
      if (codegen_comments)
        output += "// emit_reserve_stack_space\n";
      output += fmt::sprintf("sub xsp, 0u%ld;\n", ec.reserve_stack_space.size);
      break;
    case Backend::EmitCall::LOAD_FROM_STACK:
      if (codegen_comments)
        output += "// emit_load_from_stack\n";
      output += fmt::sprintf("mov %s, b%d ptr [xbp+0u%d];\n", ec.load_from_stack.load_reg, ec.load_from_stack.size * 8, (0xff - ec.load_from_stack.align + 1));
      break;
    case Backend::EmitCall::STORE_FROM_STACK:
      if (codegen_comments)
        output += "// emit_store_from_stack\n";
      output += fmt::sprintf("mov b%d ptr [xbp+0u%d], %s;\n", ec.store_from_stack.size * 8, (0xff - ec.store_from_stack.align + 1), ec.store_from_stack.rsrc);
      break;
    case Backend::EmitCall::LOADADDR_FROM_STACK:
      if (codegen_comments)
        output += "// emit_loadaddr_from_stack\n";

      output += fmt::sprintf("mov %s, xbp;\n", ec.loadaddr_from_stack.load_reg);
      output += fmt::sprintf("sub %s, %d;\n", ec.loadaddr_from_stack.load_reg, ec.loadaddr_from_stack.align);
      break;
    case Backend::EmitCall::CALL:
      if (codegen_comments)
        output += "// emit_call\n";
      output += fmt::sprintf("call %s;\n", ec.call.name);
      break;
    case Backend::EmitCall::PUSH:
      if (codegen_comments)
        output += "// emit_push\n";
      output += fmt::sprintf("push %s;\n", ec.push.reg);
      break;
    case Backend::EmitCall::PUSH_IMM:
      if (codegen_comments)
        output += "// emit_push_imm\n";
      // TODO: This doesn't work since hcasm doesn't seem to support immediate values in push, altough hcemul does support immediate pushes
      output += fmt::sprintf("push %ld;\n", ec.push_imm.val);
      break;
    case Backend::EmitCall::POP:
      if (codegen_comments)
        output += "// emit_pop\n";
      output += fmt::sprintf("pop %s;\n", ec.pop.reg);
      break;
    case Backend::EmitCall::SINGLE_RET:
      if (codegen_comments)
        output += "// emit_single_ret\n";
      output += "ret;\n";
      break;
    case Backend::EmitCall::LABEL:
      if (codegen_comments)
        output += "// emit_label\n";
      output += ec.label.name + ":\n";
      break;
    }
  }
  return output;
}
