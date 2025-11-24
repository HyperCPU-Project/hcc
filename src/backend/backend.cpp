#include "abi_metadata.hpp"
#include "ast/type.hpp"
#include "regallocator/register_allocator.hpp"
#include "type_metadata.hpp"
#include <backend/backend.hpp>
#include <fmt/printf.h>
#include <hcc.hpp>

using namespace hcc;

Backend::Backend(HCC* _hcc)
    : hcc(_hcc), register_allocator(ABIMetadata()) {
}

Backend::~Backend() {
}

std::string Backend::AllocateRegister() {
  return register_allocator.AllocateRegister();
}

void Backend::RetainRegister(std::string reg) {
  register_allocator.RetainRegister(reg);
}

void Backend::ReleaseRegister(std::string reg) {
  register_allocator.ReleaseRegister(reg);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

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
void Backend::EmitPop(std::string reg) {
}

void Backend::EmitSingleRet() {
}

void Backend::EmitLabel(std::string name) {
}

std::optional<TypeMetadata> Backend::GetType(ParserVarType type) {
  if (!types.contains(type.name)) {
    CompileError(fmt::sprintf("unknown type %s", type.name));
    return {};
  }

  TypeMetadata md = types[type.name];
  md.register_ = type.register_;
  return md;
}

void Backend::CompileError(std::string error) {
  hcc->compile_error = error;
}

#pragma GCC diagnostic pop
