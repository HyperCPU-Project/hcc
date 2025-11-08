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

void QprocBackend::EmitFunctionPrologue(std::string name) {
  struct EmitCall c;
  c.type = Backend::EmitCall::FUNCTION_PROLOGUE;
  c.function_prologue.name = name;
  emitcalls.push_back(c);
}

void QprocBackend::EmitFunctionEpilogue() {
  struct EmitCall c;
  c.type = Backend::EmitCall::FUNCTION_EPILOGUE;
  emitcalls.push_back(c);
}

std::string QprocBackend::EmitMovConst(uint64_t value, std::string reg_name) {
  struct EmitCall c;
  c.type = Backend::EmitCall::MOV_CONST;
  c.mov_const.value = value;
  c.mov_const.reg_name = reg_name;
  emitcalls.push_back(c);
  return reg_name;
}

void QprocBackend::EmitAdd(std::string ROUT, std::string RLHS, std::string RRHS) {
  struct EmitCall c;
  c.type = Backend::EmitCall::ADD;
  c.add.ROUT = ROUT;
  c.add.RLHS = RLHS;
  c.add.RRHS = RRHS;
  emitcalls.push_back(c);
}

void QprocBackend::EmitSub(std::string ROUT, std::string RLHS, std::string RRHS) {
  struct EmitCall c;
  c.type = Backend::EmitCall::SUB;
  c.sub.ROUT = ROUT;
  c.sub.RLHS = RLHS;
  c.sub.RRHS = RRHS;
  emitcalls.push_back(c);
}

void QprocBackend::EmitMul(std::string ROUT, std::string RLHS, std::string RRHS) {
  struct EmitCall c;
  c.type = Backend::EmitCall::MUL;
  c.mul.ROUT = ROUT;
  c.mul.RLHS = RLHS;
  c.mul.RRHS = RRHS;
  emitcalls.push_back(c);
}

void QprocBackend::EmitDiv(std::string ROUT, std::string RLHS, std::string RRHS) {
  struct EmitCall c;
  c.type = Backend::EmitCall::DIV;
  c.div.ROUT = ROUT;
  c.div.RLHS = RLHS;
  c.div.RRHS = RRHS;
  emitcalls.push_back(c);
}

void QprocBackend::EmitMove(std::string rdest, std::string rsrc) {
  struct EmitCall c;
  c.type = Backend::EmitCall::MOVE;
  c.move.rdest = rdest;
  c.move.rsrc = rsrc;
  emitcalls.push_back(c);
}

void QprocBackend::EmitReserveStackSpace(uint64_t size) {
  struct EmitCall c;
  c.type = Backend::EmitCall::RESERVE_STACK_SPACE;
  c.reserve_stack_space.size = size;
  emitcalls.push_back(c);
}

std::string QprocBackend::EmitLoadFromStack(uint64_t align, uint64_t size, std::string reg) {
  struct EmitCall c;
  c.type = Backend::EmitCall::LOAD_FROM_STACK;
  c.load_from_stack.align = align;
  c.load_from_stack.size = size;
  c.load_from_stack.load_reg = reg;
  emitcalls.push_back(c);
  return reg;
}

void QprocBackend::EmitStoreToStack(uint64_t align, uint64_t size, std::string rsrc) {
  struct EmitCall c;
  c.type = Backend::EmitCall::STORE_FROM_STACK;
  c.store_from_stack.align = align;
  c.store_from_stack.size = size;
  c.store_from_stack.rsrc = rsrc;
  emitcalls.push_back(c);
}

std::string QprocBackend::EmitLoadaddrFromStack(uint64_t align, std::string reg) {
  struct EmitCall c;
  c.type = Backend::EmitCall::LOADADDR_FROM_STACK;
  c.loadaddr_from_stack.align = align;
  c.loadaddr_from_stack.load_reg = reg;
  emitcalls.push_back(c);
  return reg;
}

void QprocBackend::EmitCall(std::string name) {
  struct EmitCall c;
  c.type = Backend::EmitCall::CALL;
  c.call.name = name;
  emitcalls.push_back(c);
}

void QprocBackend::EmitPush(std::string reg) {
  struct EmitCall c;
  c.type = Backend::EmitCall::PUSH;
  c.push.reg = reg;
  emitcalls.push_back(c);
}

void QprocBackend::EmitPushImm(long val) {
  struct EmitCall c;
  c.type = Backend::EmitCall::PUSH_IMM;
  c.push_imm.val = val;
  emitcalls.push_back(c);
}

void QprocBackend::EmitPop(std::string reg) {
  struct EmitCall c;
  c.type = Backend::EmitCall::POP;
  c.pop.reg = reg;
  emitcalls.push_back(c);
}

void QprocBackend::EmitSingleRet() {
  struct EmitCall c;
  c.type = Backend::EmitCall::SINGLE_RET;
  emitcalls.push_back(c);
}

void QprocBackend::EmitLabel(std::string name) {
  struct EmitCall c;
  c.type = Backend::EmitCall::LABEL;
  c.label.name = name;
  emitcalls.push_back(c);
}
