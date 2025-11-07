#include <backend/backend.hpp>
#include <fmt/printf.h>
#include <hcc.hpp>

using namespace hcc;

Backend::Backend() {
  regIndex = 0;
}

Backend::~Backend() {
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

uint64_t Backend::incrementRegIndex() {
  return 0;
}

void Backend::resetRegIndex() {
  regIndex = 0;
}

void Backend::emitFunctionPrologue(std::string name) {
}

void Backend::emitFunctionEpilogue() {
}

std::string Backend::emitMovConst(uint64_t value, std::string reg_name) {
  return "";
}

void Backend::emitAdd(std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::emitSub(std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::emitMul(std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::emitDiv(std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::emitMove(std::string rdest, std::string rsrc) {
}

void Backend::emitReserveStackSpae(uint64_t size) {
}

std::string Backend::emitLoadFromStack(uint64_t align, uint64_t size, std::string load_reg) {
  return "";
}

void Backend::emitStoreToStack(uint64_t align, uint64_t size, std::string rsrc) {
}

std::string Backend::emitLoadaddrFromStack(uint64_t align, std::string load_reg) {
  return "";
}

void Backend::emitCall(std::string name) {
}

void Backend::emitPush(std::string reg) {
}
void Backend::emitPop(std::string reg) {
}

void Backend::emitSingleRet() {
}

void Backend::emitLabel(std::string name) {
}

TypeMetadata* Backend::getTypeFromName(std::string name) {
  if (!types.contains(name)) {
    hccCompileError = fmt::sprintf("unknown type %s", name);
    return nullptr;
  }

  return &types[name];
}

#pragma GCC diagnostic pop
