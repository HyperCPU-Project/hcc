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

uint64_t Backend::increment_reg_index() {
  return 0;
}

void Backend::reset_reg_index() {
  reg_index = 0;
}

void Backend::peephole_optimize() {
}

std::string Backend::compile_calls() {
  return "";
}

void Backend::emit_function_prologue(std::string name) {
}

void Backend::emit_function_epilogue() {
}

std::string Backend::emit_mov_const(uint64_t value, std::string reg_name) {
  return "";
}

void Backend::emit_add(std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::emit_sub(std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::emit_mul(std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::emit_div(std::string ROUT, std::string RLHS, std::string RRHS) {
}

void Backend::emit_move(std::string rdest, std::string rsrc) {
}

void Backend::emit_reserve_stack_space(uint64_t size) {
}

std::string Backend::emit_load_from_stack(uint64_t align, uint64_t size, std::string load_reg) {
  return "";
}

void Backend::emit_store_from_stack(uint64_t align, uint64_t size, std::string rsrc) {
}

std::string Backend::emit_loadaddr_from_stack(uint64_t align, std::string load_reg) {
  return "";
}

void Backend::emit_call(std::string name) {
}

void Backend::emit_push(std::string reg) {
}
void Backend::emit_push_imm(long val) {
}
void Backend::emit_pop(std::string reg) {
}

void Backend::emit_single_ret() {
}

void Backend::emit_label(std::string name) {
}

TypeMetadata* Backend::get_type_from_name(std::string name) {
  if (!types.contains(name)) {
    hcc_compile_error = fmt::sprintf("unknown type %s", name);
    return nullptr;
  }

  return &types[name];
}

#pragma GCC diagnostic pop
