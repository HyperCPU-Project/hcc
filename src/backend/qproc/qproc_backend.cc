#include <backend/qproc/qproc_backend.hpp>
#include <string>

using namespace hcc;

QprocBackend::QprocBackend() {
  reg_index = 0;
  types["void"] = TypeMetadata{"void", 0};
  types["char"] = TypeMetadata{"char", 1};
  types["short"] = TypeMetadata{"short", 2};
  types["int"] = TypeMetadata{"int", 4};
  types["long"] = TypeMetadata{"long", 4}; // size of 4 is intentional here
  abi.return_register = "r0";
  for (int i = 2; i <= 12; i++) {
    abi.args_registers.push_back(fmt::format("r{}", i));
  }
  for (int i = 0; i <= 12; i++) {
    abi.registers.push_back(fmt::format("r{}", i));
  }
}

uint64_t QprocBackend::increment_reg_index() {
  uint64_t res = reg_index++;
  if (reg_index > 12) {
    reg_index = 0;
  }
  return res;
}

void QprocBackend::peephole_optimize() {
}

void QprocBackend::emit_function_prologue(std::string name) {
  EmitCall c;
  c.type = Backend::EmitCall::FUNCTION_PROLOGUE;
  c.function_prologue.name = name;
  emitcalls.push_back(c);
  /*
  if (codegen_comments)
    output += "; emit_function_prologue\n";
  output += fmt::sprintf("%s:\n", name);
  output += fmt::sprintf("push bp\nmov bp sp\n");*/
}

void QprocBackend::emit_function_epilogue() {
  EmitCall c;
  c.type = Backend::EmitCall::FUNCTION_EPILOGUE;
  emitcalls.push_back(c);
  /*
  if (codegen_comments)
    output += "; emit_function_epilogue\n";
  output += fmt::sprintf("mov sp bp\npop bp\npop ip\n");*/
}

std::string QprocBackend::emit_mov_const(uint64_t value, std::string reg_name) {
  if (reg_name == "") {
    reg_name = fmt::format("r{}", increment_reg_index());
  }

  EmitCall c;
  c.type = Backend::EmitCall::MOV_CONST;
  c.mov_const.value = value;
  c.mov_const.reg_name = reg_name;
  emitcalls.push_back(c);
  /*
  if (codegen_comments)
    output += "; emit_mov_const\n";

  output += fmt::sprintf("movi %s %ld\n", reg_name, value);
*/
  return reg_name;
}

void QprocBackend::emit_add(std::string ROUT, std::string RLHS, std::string RRHS) {
  EmitCall c;
  c.type = Backend::EmitCall::ADD;
  c.add.ROUT = ROUT;
  c.add.RLHS = RLHS;
  c.add.RRHS = RRHS;
  emitcalls.push_back(c);
  /*
  if (codegen_comments)
    output += "; emit_add\n";
  if (ROUT != RLHS) {
    output += fmt::sprintf("add %s %s\nmov %s %s\n", RLHS, RRHS, ROUT, RLHS);
    return;
  }
  output += fmt::sprintf("add %s %s\n", RLHS, RRHS);
  */
}

void QprocBackend::emit_sub(std::string ROUT, std::string RLHS, std::string RRHS) {
  EmitCall c;
  c.type = Backend::EmitCall::SUB;
  c.sub.ROUT = ROUT;
  c.sub.RLHS = RLHS;
  c.sub.RRHS = RRHS;
  emitcalls.push_back(c);
  /*
  if (codegen_comments)
    output += "; emit_sub\n";
  if (ROUT != RLHS) {
    output += fmt::sprintf("sub %s %s\nmov %s %s\n", RLHS, RRHS, ROUT, RLHS);
    return;
  }
  output += fmt::sprintf("sub %s %s\n", RLHS, RRHS);
  */
}

void QprocBackend::emit_mul(std::string ROUT, std::string RLHS, std::string RRHS) {
  EmitCall c;
  c.type = Backend::EmitCall::MUL;
  c.mul.ROUT = ROUT;
  c.mul.RLHS = RLHS;
  c.mul.RRHS = RRHS;
  emitcalls.push_back(c);
  /*
  if (codegen_comments)
    output += "; emit_mul\n";
  if (ROUT != RLHS) {
    output += fmt::sprintf("mul %s %s\nmov %s %s\n", RLHS, RRHS, ROUT, RLHS);
    return;
  }
  output += fmt::sprintf("mul %s %s\n", RLHS, RRHS);
  */
}

void QprocBackend::emit_div(std::string ROUT, std::string RLHS, std::string RRHS) {
  EmitCall c;
  c.type = Backend::EmitCall::DIV;
  c.div.ROUT = ROUT;
  c.div.RLHS = RLHS;
  c.div.RRHS = RRHS;
  emitcalls.push_back(c);
  /*
  if (codegen_comments)
    output += "; emit_div\n";
  if (ROUT != RLHS) {
    output += fmt::sprintf("div %s %s\nmov %s %s\n", RLHS, RRHS, ROUT, RLHS);
    return;
  }
  output += fmt::sprintf("div %s %s\n", RLHS, RRHS);
  */
}

void QprocBackend::emit_move(std::string rdest, std::string rsrc) {
  EmitCall c;
  c.type = Backend::EmitCall::MOVE;
  c.move.rdest = rdest;
  c.move.rsrc = rsrc;
  emitcalls.push_back(c);
  /*
  if (codegen_comments)
    output += "; emit_move\n";
  output += fmt::sprintf("mov %s %s\n", rdest, rsrc);
  */
}

void QprocBackend::emit_reserve_stack_space(uint64_t size) {
  EmitCall c;
  c.type = Backend::EmitCall::RESERVE_STACK_SPACE;
  c.reserve_stack_space.size = size;
  emitcalls.push_back(c);
  /*
  if (codegen_comments)
    output += "; emit_reserve_stack_space\n";
  output += fmt::sprintf("movi r0 %ld\nsub sp, r0\n", size);
  */
}

std::string QprocBackend::emit_load_from_stack(uint64_t align, uint64_t size, std::string reg) {
  if (reg.empty()) {
    reg = "r" + std::to_string(increment_reg_index());
    while (reg == "r0" || reg == "r1")
      reg = "r" + std::to_string(increment_reg_index());
  }
  EmitCall c;
  c.type = Backend::EmitCall::LOAD_FROM_STACK;
  c.load_from_stack.align = align;
  c.load_from_stack.size = size;
  c.load_from_stack.load_reg = reg;
  emitcalls.push_back(c);

  /*
  if (codegen_comments)
    output += "; emit_load_from_stack\n";
  output += fmt::sprintf("mov r0 bp\n");
  output += fmt::sprintf("movi r1 %d\n", align);
  output += fmt::sprintf("sub r0 r1\n");
  if (size == 1)
    output += fmt::sprintf("lod %s byte r0\n", reg);
  if (size == 2)
    output += fmt::sprintf("lod %s word r0\n", reg);
  else
    output += fmt::sprintf("lod %s dword r0\n", reg);
    */
  return reg;
}

void QprocBackend::emit_store_from_stack(uint64_t align, uint64_t size, std::string rsrc) {
  EmitCall c;
  c.type = Backend::EmitCall::STORE_FROM_STACK;
  c.store_from_stack.align = align;
  c.store_from_stack.size = size;
  c.store_from_stack.rsrc = rsrc;
  emitcalls.push_back(c);

  /*
  if (codegen_comments)
    output += "; emit_store_from_stack\n";
  bool is_used_reg = (rsrc == "r0" || rsrc == "r1");

  if (is_used_reg)
    output += fmt::sprintf("push %s\n", rsrc);
  output += fmt::sprintf("mov r0 bp\n");
  output += fmt::sprintf("movi r1 %d\n", align);
  output += fmt::sprintf("sub r0 r1\n");
  if (rsrc == "r0") {
    output += fmt::sprintf("  ");
    is_used_reg = true;
    rsrc = "r1";
  }
  if (is_used_reg)
    output += fmt::sprintf("pop %s\n", rsrc);

  if (size == 1)
    output += fmt::sprintf("str byte r0 %s\n", rsrc);
  if (size == 2)
    output += fmt::sprintf("str word r0 %s\n", rsrc);
  else
    output += fmt::sprintf("str dword r0 %s\n", rsrc);
    */
}

std::string QprocBackend::emit_loadaddr_from_stack(uint64_t align, std::string reg) {
  if (reg.empty())
    reg = std::to_string(increment_reg_index());
  if (reg == "r0")
    reg = std::to_string(increment_reg_index());
  reg = "r" + reg;

  EmitCall c;
  c.type = Backend::EmitCall::LOADADDR_FROM_STACK;
  c.loadaddr_from_stack.align = align;
  c.loadaddr_from_stack.load_reg = reg;
  emitcalls.push_back(c);

  /*
  if (codegen_comments)
    output += "; emit_loadaddr_from_stack\n";

  output += fmt::sprintf("mov %s bp\n", reg);
  output += fmt::sprintf("movi r0 %d\n", align);
  output += fmt::sprintf("sub %s r0\n", reg);
  */
  return reg;
}

void QprocBackend::emit_call(std::string name) {
  EmitCall c;
  c.type = Backend::EmitCall::CALL;
  c.call.name = name;
  emitcalls.push_back(c);
  /*
  if (codegen_comments)
    output += "; emit_call\n";
  output += fmt::sprintf("call %s\n", name);
  */
}

void QprocBackend::emit_push(std::string reg) {
  EmitCall c;
  c.type = Backend::EmitCall::PUSH;
  c.push.reg = reg;
  emitcalls.push_back(c);
  /*
  if (codegen_comments)
    output += "; emit_push\n";
  output += fmt::sprintf("push %s\n", reg);
  */
}

void QprocBackend::emit_push_imm(long val) {
  if (codegen_comments)
    output += "; emit_push_imm\n";
  output += fmt::sprintf("push %ld\n", val);
}

void QprocBackend::emit_pop(std::string reg) {
  EmitCall c;
  c.type = Backend::EmitCall::POP;
  c.pop.reg = reg;
  emitcalls.push_back(c);
  /*
  if (codegen_comments)
    output += "; emit_pop\n";
  output += fmt::sprintf("pop %s\n", reg);
  */
}

void QprocBackend::emit_single_ret() {
  EmitCall c;
  c.type = Backend::EmitCall::SINGLE_RET;
  emitcalls.push_back(c);
  /*
  if (codegen_comments)
    output += "// emit_single_ret\n";
  output += "pop ip\n";
  */
}

void QprocBackend::emit_label(std::string name) {
  EmitCall c;
  c.type = Backend::EmitCall::LABEL;
  c.label.name = name;
  emitcalls.push_back(c);
  /*
  if (codegen_comments)
    output += "// emit_label\n";
  output += name + ":\n";
  */
}
