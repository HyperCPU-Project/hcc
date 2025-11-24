#include <backend/hypercpu/hypercpu_backend.hpp>
#include <dep_pch.hpp>

using namespace hcc;

HyperCPUBackend::HyperCPUBackend(HCC* hcc)
    : Backend(hcc) {
  types["void"] = TypeMetadata{"void", 0};
  types["char"] = TypeMetadata{"char", 1};
  types["short"] = TypeMetadata{"short", 2};
  types["int"] = TypeMetadata{"int", 4};
  types["long"] = TypeMetadata{"long", 8};
  abi.return_register = "x0";
  for (int i = 0; i <= 7; i++) {
    abi.registers.push_back(fmt::format("x{}", i));
  }
  for (int i = 2; i <= 7; i++) {
    abi.args_registers.push_back(fmt::format("x{}", i));
  }
  register_allocator = RegisterAllocator(abi);
}

void HyperCPUBackend::EmitFunctionPrologue(std::string name) {
  if (codegen_comments)
    output += "// emit_function_prologue\n";
  output += fmt::sprintf("%s:\n", name);
  output += fmt::sprintf("push xbp;\nmov xbp, xsp;\n");
}

void HyperCPUBackend::EmitFunctionEpilogue() {
  if (codegen_comments)
    output += "// emit_function_epilogue\n";
  output += fmt::sprintf("mov xsp, xbp;\npop xbp;\nret;\n");
}

std::string HyperCPUBackend::EmitMovConst(uint64_t value, std::string reg_name) {
  if (codegen_comments)
    output += "// emit_mov_const\n";

  output += fmt::sprintf("mov %s, 0u%ld;\n", reg_name, value);

  return reg_name;
}

void HyperCPUBackend::EmitAdd(std::string ROUT, std::string RLHS, std::string RRHS) {
  if (codegen_comments)
    output += "// emit_add\n";
  if (ROUT != RLHS) {
    output += fmt::sprintf("add %s, %s;\nmov %s, %s;\n", RLHS, RRHS, ROUT, RLHS);
    return;
  }
  output += fmt::sprintf("add %s, %s;\n", RLHS, RRHS);
}

void HyperCPUBackend::EmitSub(std::string ROUT, std::string RLHS, std::string RRHS) {
  if (codegen_comments)
    output += "// emit_sub\n";
  if (ROUT != RLHS) {
    output += fmt::sprintf("sub %s, %s;\nmov %s, %s;\n", RLHS, RRHS, ROUT, RLHS);
    return;
  }
  output += fmt::sprintf("sub %s, %s;\n", RLHS, RRHS);
}

void HyperCPUBackend::EmitMul(std::string ROUT, std::string RLHS, std::string RRHS) {
  if (codegen_comments)
    output += "// emit_mul\n";
  if (ROUT != RLHS) {
    output += fmt::sprintf("mul %s, %s;\nmov %s, %s;\n", RLHS, RRHS, ROUT, RLHS);
    return;
  }
  output += fmt::sprintf("mul %s, %s;\n", RLHS, RRHS);
}

void HyperCPUBackend::EmitDiv(std::string ROUT, std::string RLHS, std::string RRHS) {
  if (codegen_comments)
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

void HyperCPUBackend::EmitMove(std::string rdest, std::string rsrc) {
  if (rdest != rsrc) {
    if (codegen_comments)
      output += "// emit_move\n";
    output += fmt::sprintf("mov %s, %s;\n", rdest, rsrc);
  }
}

void HyperCPUBackend::EmitReserveStackSpace(uint64_t size) {
  if (codegen_comments)
    output += "// emit_reserve_stack_space\n";
  output += fmt::sprintf("sub xsp, 0u%ld;\n", size);
}

std::string HyperCPUBackend::EmitLoadFromStack(uint64_t align, uint64_t size, std::string reg) {
  if (codegen_comments)
    output += "// emit_load_from_stack\n";
  output += fmt::sprintf("mov %s, b%d ptr [xbp+0u%d];\n", reg, size * 8, (0xff - align + 1));
  return reg;
}

void HyperCPUBackend::EmitStoreToStack(uint64_t align, uint64_t size, std::string rsrc) {
  if (codegen_comments)
    output += "// emit_store_from_stack\n";
  output += fmt::sprintf("mov b%d ptr [xbp+0u%d], %s;\n", size * 8, (0xff - align + 1), rsrc);
}

std::string HyperCPUBackend::EmitLoadaddrFromStack(uint64_t align, std::string reg) {
  if (codegen_comments)
    output += "// emit_loadaddr_from_stack\n";

  output += fmt::sprintf("mov %s, xbp;\n", reg);
  output += fmt::sprintf("sub %s, %d;\n", reg, align);
  return reg;
}

void HyperCPUBackend::EmitCall(std::string name) {
  if (codegen_comments)
    output += "// emit_call\n";
  output += fmt::sprintf("call %s;\n", name);
}

void HyperCPUBackend::EmitPush(std::string reg) {
  if (codegen_comments)
    output += "// emit_push\n";
  output += fmt::sprintf("push %s;\n", reg);
}

void HyperCPUBackend::EmitPop(std::string reg) {
  if (codegen_comments)
    output += "// emit_pop\n";
  output += fmt::sprintf("pop %s;\n", reg);
}

void HyperCPUBackend::EmitSingleRet() {
  if (codegen_comments)
    output += "// emit_single_ret\n";
  output += "ret;\n";
}

void HyperCPUBackend::EmitLabel(std::string name) {
  if (codegen_comments)
    output += "// emit_label\n";
  output += name + ":\n";
}
