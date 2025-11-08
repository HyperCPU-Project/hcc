#include <backend/backend.hpp>
#include <fmt/printf.h>
#include <hcc.hpp>

using namespace hcc;

Backend::Backend() {
  reg_index = 0;
}

Backend::~Backend() {
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

std::string Backend::CompileCalls() {
  return "";
}

void Backend::EmitFunctionPrologue(std::string name) {
}

void Backend::EmitFunctionEpilogue() {
}

std::string Backend::EmitMovConst(uint64_t value, std::string reg_name) {
  return "";
}

void Backend::EmitAdd(std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::EmitSub(std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::EmitMul(std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::EmitDiv(std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::EmitMove(std::string rdest, std::string rsrc) {
}

void Backend::EmitReserveStackSpace(uint64_t size) {
}

std::string Backend::EmitLoadFromStack(uint64_t align, uint64_t size, std::string load_reg) {
  return "";
}

void Backend::EmitStoreToStack(uint64_t align, uint64_t size, std::string rsrc) {
}

std::string Backend::EmitLoadaddrFromStack(uint64_t align, std::string load_reg) {
  return "";
}

void Backend::EmitCall(std::string name) {
}

void Backend::EmitPush(std::string reg) {
}
void Backend::EmitPushImm(long val) {
}
void Backend::EmitPop(std::string reg) {
}

void Backend::EmitSingleRet() {
}

void Backend::EmitLabel(std::string name) {
}

TypeMetadata* Backend::GetTypeFromName(std::string name) {
  if (!types.contains(name)) {
    hcc_compile_error = fmt::sprintf("unknown type %s", name);
    return nullptr;
  }

  return &types[name];
}

#pragma GCC diagnostic pop
