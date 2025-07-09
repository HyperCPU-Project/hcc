#include <backend/hypercpu/hypercpu_backend.hpp>

using namespace hcc;

HyperCPUBackend::HyperCPUBackend() {
  reg_index = 0;
  types["void"] = TypeMetadata{"void", 0};
  types["char"] = TypeMetadata{"char", 1};
  types["short"] = TypeMetadata{"short", 2};
  types["int"] = TypeMetadata{"int", 4};
  types["long"] = TypeMetadata{"long", 8}; // size of 4 is intentional here
  abi.return_register = "x0";
  for (int i = 2; i <= 7; i++) {
    abi.args_registers.push_back(fmt::format("x{}", i));
  }
}

uint64_t HyperCPUBackend::increment_reg_index() {
  uint64_t res = reg_index++;
  if (reg_index > 7) {
    reg_index = 0;
  }
  return res;
}

void HyperCPUBackend::peephole_optimize() {
}

void HyperCPUBackend::emit_function_prologue(std::string name) {
  EmitCall c;
  c.type = Backend::EmitCall::FUNCTION_PROLOGUE;
  c.function_prologue.name = name;
  emitcalls.push_back(c);
}

void HyperCPUBackend::emit_function_epilogue() {
  EmitCall c;
  c.type = Backend::EmitCall::FUNCTION_EPILOGUE;
  emitcalls.push_back(c);
}

std::string HyperCPUBackend::emit_mov_const(uint64_t value, std::string reg_name) {
  if (reg_name == "") {
    reg_name = fmt::format("x{}", increment_reg_index());
  }

  EmitCall c;
  c.type = Backend::EmitCall::MOV_CONST;
  c.mov_const.value = value;
  c.mov_const.reg_name = reg_name;
  emitcalls.push_back(c);
  return reg_name;
}

void HyperCPUBackend::emit_add(std::string ROUT, std::string RLHS, std::string RRHS) {
  EmitCall c;
  c.type = Backend::EmitCall::ADD;
  c.add.ROUT = ROUT;
  c.add.RLHS = RLHS;
  c.add.RRHS = RRHS;
  emitcalls.push_back(c);
}

void HyperCPUBackend::emit_sub(std::string ROUT, std::string RLHS, std::string RRHS) {
  EmitCall c;
  c.type = Backend::EmitCall::SUB;
  c.sub.ROUT = ROUT;
  c.sub.RLHS = RLHS;
  c.sub.RRHS = RRHS;
  emitcalls.push_back(c);
}

void HyperCPUBackend::emit_mul(std::string ROUT, std::string RLHS, std::string RRHS) {
  EmitCall c;
  c.type = Backend::EmitCall::MUL;
  c.mul.ROUT = ROUT;
  c.mul.RLHS = RLHS;
  c.mul.RRHS = RRHS;
  emitcalls.push_back(c);
}

void HyperCPUBackend::emit_div(std::string ROUT, std::string RLHS, std::string RRHS) {
  EmitCall c;
  c.type = Backend::EmitCall::DIV;
  c.div.ROUT = ROUT;
  c.div.RLHS = RLHS;
  c.div.RRHS = RRHS;
  emitcalls.push_back(c);
}

void HyperCPUBackend::emit_move(std::string rdest, std::string rsrc) {
  EmitCall c;
  c.type = Backend::EmitCall::MOVE;
  c.move.rdest = rdest;
  c.move.rsrc = rsrc;
  emitcalls.push_back(c);
}

void HyperCPUBackend::emit_reserve_stack_space(uint64_t size) {
  EmitCall c;
  c.type = Backend::EmitCall::RESERVE_STACK_SPACE;
  c.reserve_stack_space.size = size;
  emitcalls.push_back(c);
}

std::string HyperCPUBackend::emit_load_from_stack(uint64_t align, uint64_t size, std::string reg) {
  if (reg.empty()) {
    reg = "x" + std::to_string(increment_reg_index());
  }

  EmitCall c;
  c.type = Backend::EmitCall::LOAD_FROM_STACK;
  c.load_from_stack.align = align;
  c.load_from_stack.size = size;
  c.load_from_stack.load_reg = reg;
  emitcalls.push_back(c);
  return reg;
}

void HyperCPUBackend::emit_store_from_stack(uint64_t align, uint64_t size, std::string rsrc) {
  EmitCall c;
  c.type = Backend::EmitCall::STORE_FROM_STACK;
  c.store_from_stack.align = align;
  c.store_from_stack.size = size;
  c.store_from_stack.rsrc = rsrc;
  emitcalls.push_back(c);
}

std::string HyperCPUBackend::emit_loadaddr_from_stack(uint64_t align, std::string reg) {

  if (reg.empty())
    reg = std::to_string(increment_reg_index());
  if (reg == "x0")
    reg = std::to_string(increment_reg_index());
  reg = "x" + reg;

  EmitCall c;
  c.type = Backend::EmitCall::LOADADDR_FROM_STACK;
  c.loadaddr_from_stack.align = align;
  c.loadaddr_from_stack.load_reg = reg;
  emitcalls.push_back(c);
  return reg;
}

void HyperCPUBackend::emit_call(std::string name) {
  EmitCall c;
  c.type = Backend::EmitCall::CALL;
  c.call.name = name;
  emitcalls.push_back(c);
}

void HyperCPUBackend::emit_push(std::string reg) {
  EmitCall c;
  c.type = Backend::EmitCall::PUSH;
  c.push.reg = reg;
  emitcalls.push_back(c);
}

void HyperCPUBackend::emit_pop(std::string reg) {
  EmitCall c;
  c.type = Backend::EmitCall::POP;
  c.pop.reg = reg;
  emitcalls.push_back(c);
}

void HyperCPUBackend::emit_single_ret() {
  EmitCall c;
  c.type = Backend::EmitCall::SINGLE_RET;
  emitcalls.push_back(c);
}

void HyperCPUBackend::emit_label(std::string name) {
  EmitCall c;
  c.type = Backend::EmitCall::LABEL;
  c.label.name = name;
  emitcalls.push_back(c);
}
