#include <backend/qproc/qproc_backend.hpp>

using namespace hcc;

std::string QprocBackend::CompileCalls() {
  std::string output;
  for (struct hcc::Backend::EmitCall ec : emitcalls) {
    switch (ec.type) {
    case Backend::EmitCall::FUNCTION_PROLOGUE:
      if (codegen_comments)
        output += "; emit_function_prologue\n";
      output += fmt::sprintf("%s:\n", ec.function_prologue.name);
      output += fmt::sprintf("push bp\nmov bp sp\n");
      break;
    case Backend::EmitCall::FUNCTION_EPILOGUE:
      if (codegen_comments)
        output += "; emit_function_epilogue\n";
      output += fmt::sprintf("mov sp bp\npop bp\npop ip\n");
      break;
    case Backend::EmitCall::MOV_CONST:
      if (codegen_comments)
        output += "; emit_mov_const\n";

      output += fmt::sprintf("movi %s %ld\n", ec.mov_const.reg_name, ec.mov_const.value);
      break;
    case Backend::EmitCall::ADD:
      if (codegen_comments)
        output += "; emit_add\n";
      if (ec.add.ROUT != ec.add.RLHS) {
        output += fmt::sprintf("add %s %s\nmov %s %s\n", ec.add.RLHS, ec.add.RRHS, ec.add.ROUT, ec.add.RLHS);
      } else
        output += fmt::sprintf("add %s %s\n", ec.add.RLHS, ec.add.RRHS);
      break;
    case Backend::EmitCall::SUB:
      if (codegen_comments)
        output += "; emit_sub\n";
      if (ec.sub.ROUT != ec.sub.RLHS) {
        output += fmt::sprintf("sub %s %s\nmov %s %s\n", ec.sub.RLHS, ec.sub.RRHS, ec.sub.ROUT, ec.sub.RLHS);
      } else
        output += fmt::sprintf("sub %s %s\n", ec.sub.RLHS, ec.sub.RRHS);
      break;
    case Backend::EmitCall::MUL:
      if (codegen_comments)
        output += "; emit_mul\n";
      if (ec.mul.ROUT != ec.mul.RLHS) {
        output += fmt::sprintf("mul %s %s\nmov %s %s\n", ec.mul.RLHS, ec.mul.RRHS, ec.mul.ROUT, ec.mul.RLHS);
      } else
        output += fmt::sprintf("mul %s %s\n", ec.mul.RLHS, ec.mul.RRHS);
      break;
    case Backend::EmitCall::DIV:
      if (codegen_comments)
        output += "; emit_div\n";
      if (ec.div.ROUT != ec.div.RLHS) {
        output += fmt::sprintf("div %s %s\nmov %s %s\n", ec.div.RLHS, ec.div.RRHS, ec.div.ROUT, ec.div.RLHS);
      } else
        output += fmt::sprintf("div %s %s\n", ec.div.RLHS, ec.div.RRHS);
      break;
    case Backend::EmitCall::MOVE:
      if (ec.move.rdest != ec.move.rsrc) {
        if (codegen_comments)
          output += "; emit_move\n";
        output += fmt::sprintf("mov %s %s\n", ec.move.rdest, ec.move.rsrc);
      }
      break;
    case Backend::EmitCall::RESERVE_STACK_SPACE:
      if (codegen_comments)
        output += "; emit_reserve_stack_space\n";
      output += fmt::sprintf("movi r0 %ld\nsub sp, r0\n", ec.reserve_stack_space.size);
      break;
    case Backend::EmitCall::LOAD_FROM_STACK:
      if (codegen_comments)
        output += "; emit_load_from_stack\n";
      output += fmt::sprintf("mov r2 bp\n");
      output += fmt::sprintf("movi r3 %d\n", ec.load_from_stack.align);
      output += fmt::sprintf("sub r2 r3\n");
      if (ec.load_from_stack.size == 1)
        output += fmt::sprintf("lod %s byte r2\n", ec.load_from_stack.load_reg);
      if (ec.load_from_stack.size == 2)
        output += fmt::sprintf("lod %s word r2\n", ec.load_from_stack.load_reg);
      else
        output += fmt::sprintf("lod %s dword r2\n", ec.load_from_stack.load_reg);
      break;
    case Backend::EmitCall::STORE_FROM_STACK: {
      if (codegen_comments)
        output += "; emit_store_from_stack\n";
      bool is_used_reg = (ec.store_from_stack.rsrc == "r2" || ec.store_from_stack.rsrc == "r3");

      if (is_used_reg)
        output += fmt::sprintf("push %s\n", ec.store_from_stack.rsrc);
      output += fmt::sprintf("mov r2 bp\n");
      output += fmt::sprintf("movi r3 %d\n", ec.store_from_stack.align);
      output += fmt::sprintf("sub r2 r3\n");
      if (ec.store_from_stack.rsrc == "r2") {
        output += fmt::sprintf("  ");
        is_used_reg = true;
        ec.store_from_stack.rsrc = "r3";
      }
      if (is_used_reg)
        output += fmt::sprintf("pop %s\n", ec.store_from_stack.rsrc);

      if (ec.store_from_stack.size == 1)
        output += fmt::sprintf("str byte r2 %s\n", ec.store_from_stack.rsrc);
      if (ec.store_from_stack.size == 2)
        output += fmt::sprintf("str word r2 %s\n", ec.store_from_stack.rsrc);
      else
        output += fmt::sprintf("str dword r2 %s\n", ec.store_from_stack.rsrc);
    } break;
    case Backend::EmitCall::LOADADDR_FROM_STACK:
      if (codegen_comments)
        output += "; emit_loadaddr_from_stack\n";

      output += fmt::sprintf("mov %s bp\n", ec.loadaddr_from_stack.load_reg);
      output += fmt::sprintf("movi r0 %d\n", ec.loadaddr_from_stack.align);
      output += fmt::sprintf("sub %s r0\n", ec.loadaddr_from_stack.load_reg);
      break;
    case Backend::EmitCall::CALL:
      if (codegen_comments)
        output += "; emit_call\n";
      output += fmt::sprintf("call %s\n", ec.call.name);
      break;
    case Backend::EmitCall::PUSH:
      if (codegen_comments)
        output += "; emit_push\n";
      output += fmt::sprintf("push %s\n", ec.push.reg);
      break;
    case Backend::EmitCall::PUSH_IMM:
      if (codegen_comments)
        output += "; emit_push_imm\n";
      output += fmt::sprintf("push %ld\n", ec.push_imm.val);
      break;
    case Backend::EmitCall::POP:
      if (codegen_comments)
        output += "; emit_pop\n";
      output += fmt::sprintf("pop %s\n", ec.pop.reg);
      break;
    case Backend::EmitCall::SINGLE_RET:
      if (codegen_comments)
        output += "// emit_single_ret\n";
      output += "pop ip\n";
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
