#include <backend/qproc/qproc_backend.hpp>
#include <string>

using namespace hcc;

QprocBackend::QprocBackend() {
  regIndex = 0;
  types["void"] = TypeMetadata{"void", 0};
  types["char"] = TypeMetadata{"char", 1};
  types["short"] = TypeMetadata{"short", 2};
  types["int"] = TypeMetadata{"int", 4};
  types["long"] = TypeMetadata{"long", 4}; // size of 4 is intentional here
  abi.returnRegister = "r0";
  for (int i = 2; i <= 12; i++) {
    abi.argsRegisters.push_back(fmt::format("r{}", i));
  }
}

uint64_t QprocBackend::incrementRegIndex() {
  uint64_t res = regIndex++;
  if (regIndex > 12) {
    regIndex = 0;
  }
  return res;
}

void QprocBackend::emitFunctionPrologue(std::string name) {
  if (codegenComments)
    output += "; emit_function_prologue\n";
  output += fmt::sprintf("%s:\n", name);
  output += fmt::sprintf("push bp\nmov bp sp\n");
}

void QprocBackend::emitFunctionEpilogue() {
  if (codegenComments)
    output += "; emit_function_epilogue\n";
  output += fmt::sprintf("mov sp bp\npop bp\npop ip\n");
}

std::string QprocBackend::emitMovConst(uint64_t value, std::string reg_name) {
  if (codegenComments)
    output += "; emit_mov_const\n";
  if (reg_name == "") {
    reg_name = fmt::format("r{}", incrementRegIndex());
  }

  output += fmt::sprintf("movi %s %ld\n", reg_name, value);

  return reg_name;
}

void QprocBackend::emitAdd(std::string ROUT, std::string RLHS, std::string RRHS) {
  if (codegenComments)
    output += "; emit_add\n";
  if (ROUT != RLHS) {
    output += fmt::sprintf("add %s %s\nmov %s %s\n", RLHS, RRHS, ROUT, RLHS);
    return;
  }
  output += fmt::sprintf("add %s %s\n", RLHS, RRHS);
}

void QprocBackend::emitSub(std::string ROUT, std::string RLHS, std::string RRHS) {
  if (codegenComments)
    output += "; emit_sub\n";
  if (ROUT != RLHS) {
    output += fmt::sprintf("sub %s %s\nmov %s %s\n", RLHS, RRHS, ROUT, RLHS);
    return;
  }
  output += fmt::sprintf("sub %s %s\n", RLHS, RRHS);
}

void QprocBackend::emitMul(std::string ROUT, std::string RLHS, std::string RRHS) {
  if (codegenComments)
    output += "; emit_mul\n";
  if (ROUT != RLHS) {
    output += fmt::sprintf("mul %s %s\nmov %s %s\n", RLHS, RRHS, ROUT, RLHS);
    return;
  }
  output += fmt::sprintf("mul %s %s\n", RLHS, RRHS);
}

void QprocBackend::emitDiv(std::string ROUT, std::string RLHS, std::string RRHS) {
  if (codegenComments)
    output += "; emit_div\n";
  if (ROUT != RLHS) {
    output += fmt::sprintf("div %s %s\nmov %s %s\n", RLHS, RRHS, ROUT, RLHS);
    return;
  }
  output += fmt::sprintf("div %s %s\n", RLHS, RRHS);
}

void QprocBackend::emitMove(std::string rdest, std::string rsrc) {
  if (codegenComments)
    output += "; emit_move\n";
  output += fmt::sprintf("mov %s %s\n", rdest, rsrc);
}

void QprocBackend::emitReserveStackSpae(uint64_t size) {
  if (codegenComments)
    output += "; emit_reserve_stack_space\n";
  output += fmt::sprintf("movi r0 %ld\nsub sp, r0\n", size);
}

std::string QprocBackend::emitLoadFromStack(uint64_t align, uint64_t size, std::string reg) {
  if (codegenComments)
    output += "; emit_load_from_stack\n";
  if (reg.empty()) {
    reg = "r" + std::to_string(incrementRegIndex());
    while (reg == "r0" || reg == "r1")
      reg = "r" + std::to_string(incrementRegIndex());
  }
  output += fmt::sprintf("mov r0 bp\n");
  output += fmt::sprintf("movi r1 %d\n", align);
  output += fmt::sprintf("sub r0 r1\n");
  if (size == 1)
    output += fmt::sprintf("lod %s byte r0\n", reg);
  if (size == 2)
    output += fmt::sprintf("lod %s word r0\n", reg);
  else
    output += fmt::sprintf("lod %s dword r0\n", reg);
  return reg;
}

void QprocBackend::emitStoreToStack(uint64_t align, uint64_t size, std::string rsrc) {
  if (codegenComments)
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
}

std::string QprocBackend::emitLoadaddrFromStack(uint64_t align, std::string reg) {
  if (codegenComments)
    output += "; emit_loadaddr_from_stack\n";
  if (reg.empty())
    reg = std::to_string(incrementRegIndex());
  if (reg == "r0")
    reg = std::to_string(incrementRegIndex());
  reg = "r" + reg;

  output += fmt::sprintf("mov %s bp\n", reg);
  output += fmt::sprintf("movi r0 %d\n", align);
  output += fmt::sprintf("sub %s r0\n", reg);
  return reg;
}

void QprocBackend::emitCall(std::string name) {
  if (codegenComments)
    output += "; emit_call\n";
  output += fmt::sprintf("call %s\n", name);
}

void QprocBackend::emitPush(std::string reg) {
  if (codegenComments)
    output += "; emit_push\n";
  output += fmt::sprintf("push %s\n", reg);
}

void QprocBackend::emitPop(std::string reg) {
  if (codegenComments)
    output += "; emit_pop\n";
  output += fmt::sprintf("pop %s\n", reg);
}

void QprocBackend::emitSingleRet() {
  if (codegenComments)
    output += "// emit_single_ret\n";
  output += "pop ip\n";
}

void QprocBackend::emitLabel(std::string name) {
  if (codegenComments)
    output += "// emit_label\n";
  output += name + ":\n";
}
