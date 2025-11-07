#include <backend/hypercpu/hypercpu_backend.hpp>

using namespace hcc;

HyperCPUBackend::HyperCPUBackend() {
  regIndex = 0;
  types["void"] = TypeMetadata{"void", 0};
  types["char"] = TypeMetadata{"char", 1};
  types["short"] = TypeMetadata{"short", 2};
  types["int"] = TypeMetadata{"int", 4};
  types["long"] = TypeMetadata{"long", 8};
  abi.returnRegister = "x0";
  for (int i = 2; i <= 7; i++) {
    abi.argsRegisters.push_back(fmt::format("x{}", i));
  }
}

uint64_t HyperCPUBackend::incrementRegIndex() {
  uint64_t res = regIndex++;
  if (regIndex > 7) {
    regIndex = 0;
  }
  return res;
}

void HyperCPUBackend::emitFunctionPrologue(std::string name) {
  if (codegenComments)
    output += "// emit_function_prologue\n";
  output += fmt::sprintf("%s:\n", name);
  output += fmt::sprintf("push xbp;\nmov xbp, xsp;\n");
}

void HyperCPUBackend::emitFunctionEpilogue() {
  if (codegenComments)
    output += "// emit_function_epilogue\n";
  output += fmt::sprintf("mov xsp, xbp;\npop xbp;\nret;\n");
}

std::string HyperCPUBackend::emitMovConst(uint64_t value, std::string reg_name) {
  if (codegenComments)
    output += "// emit_mov_const\n";
  if (reg_name == "") {
    reg_name = fmt::format("x{}", incrementRegIndex());
  }

  output += fmt::sprintf("mov %s, 0u%ld;\n", reg_name, value);

  return reg_name;
}

void HyperCPUBackend::emitAdd(std::string ROUT, std::string RLHS, std::string RRHS) {
  if (codegenComments)
    output += "// emit_add\n";
  if (ROUT != RLHS) {
    output += fmt::sprintf("add %s, %s;\nmov %s, %s;\n", RLHS, RRHS, ROUT, RLHS);
    return;
  }
  output += fmt::sprintf("add %s, %s;\n", RLHS, RRHS);
}

void HyperCPUBackend::emitSub(std::string ROUT, std::string RLHS, std::string RRHS) {
  if (codegenComments)
    output += "// emit_sub\n";
  if (ROUT != RLHS) {
    output += fmt::sprintf("sub %s, %s;\nmov %s, %s;\n", RLHS, RRHS, ROUT, RLHS);
    return;
  }
  output += fmt::sprintf("sub %s, %s;\n", RLHS, RRHS);
}

void HyperCPUBackend::emitMul(std::string ROUT, std::string RLHS, std::string RRHS) {
  if (codegenComments)
    output += "// emit_mul\n";
  if (ROUT != RLHS) {
    output += fmt::sprintf("mul %s, %s;\nmov %s, %s;\n", RLHS, RRHS, ROUT, RLHS);
    return;
  }
  output += fmt::sprintf("mul %s, %s;\n", RLHS, RRHS);
}

void HyperCPUBackend::emitDiv(std::string ROUT, std::string RLHS, std::string RRHS) {
  if (codegenComments)
    output += "// emit_div\n";
  // RLHS - x0
  // RRHS - x2
  output += fmt::sprintf("push x1;\n", RLHS);
  if (RRHS != "x2")
    output += fmt::sprintf("mov x2, %s;\n", RRHS);
  output += fmt::sprintf("div %s;\n", RLHS);
  output += fmt::sprintf("pop x1;\n", RLHS);
  if (ROUT != RLHS) {
    output += fmt::sprintf("mov %s, %s;\n", RLHS, ROUT);
  }
}

void HyperCPUBackend::emitMove(std::string rdest, std::string rsrc) {
  if (rdest != rsrc) {
    if (codegenComments)
      output += "// emit_move\n";
    output += fmt::sprintf("mov %s, %s;\n", rdest, rsrc);
  }
}

void HyperCPUBackend::emitReserveStackSpae(uint64_t size) {
  if (codegenComments)
    output += "// emit_reserve_stack_space\n";
  output += fmt::sprintf("sub xsp, 0u%ld;\n", size);
}

std::string HyperCPUBackend::emitLoadFromStack(uint64_t align, uint64_t size, std::string reg) {
  if (codegenComments)
    output += "// emit_load_from_stack\n";
  if (reg.empty()) {
    reg = "x" + std::to_string(incrementRegIndex());
  }
  output += fmt::sprintf("mov %s, b%d ptr [xbp+0u%d];\n", reg, size * 8, (0xff - align + 1));
  return reg;
}

void HyperCPUBackend::emitStoreToStack(uint64_t align, uint64_t size, std::string rsrc) {
  if (codegenComments)
    output += "// emit_store_from_stack\n";
  output += fmt::sprintf("mov b%d ptr [xbp+0u%d], %s;\n", size * 8, (0xff - align + 1), rsrc);
}

std::string HyperCPUBackend::emitLoadaddrFromStack(uint64_t align, std::string reg) {
  if (codegenComments)
    output += "// emit_loadaddr_from_stack\n";
  if (reg.empty())
    reg = std::to_string(incrementRegIndex());
  if (reg == "r0")
    reg = std::to_string(incrementRegIndex());
  reg = "r" + reg;

  output += fmt::sprintf("mov %s, xbp;\n", reg);
  output += fmt::sprintf("sub %s, %d;\n", reg, align);
  return reg;
}

void HyperCPUBackend::emitCall(std::string name) {
  if (codegenComments)
    output += "// emit_call\n";
  output += fmt::sprintf("call %s;\n", name);
}

void HyperCPUBackend::emitPush(std::string reg) {
  if (codegenComments)
    output += "// emit_push\n";
  output += fmt::sprintf("push %s;\n", reg);
}

void HyperCPUBackend::emitPop(std::string reg) {
  if (codegenComments)
    output += "// emit_pop\n";
  output += fmt::sprintf("pop %s;\n", reg);
}

void HyperCPUBackend::emitSingleRet() {
  if (codegenComments)
    output += "// emit_single_ret\n";
  output += "ret;\n";
}

void HyperCPUBackend::emitLabel(std::string name) {
  if (codegenComments)
    output += "// emit_label\n";
  output += name + ":\n";
}
