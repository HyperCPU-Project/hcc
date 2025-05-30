#pragma once

#include <backend/backend.hpp>

namespace hcc {
  class HyperCPUBackend : public Backend {
  public:
    HyperCPUBackend();

    uint64_t increment_reg_index();

    void peephole_optimize() override;
    std::string compile_calls() override;

    void emit_function_prologue(std::string name) override;
    void emit_function_epilogue() override;

    std::string emit_mov_const(uint64_t value, std::string reg_name = "") override;

    void emit_add(std::string ROUT, std::string RLHS, std::string RRHS) override;
    void emit_sub(std::string ROUT, std::string RLHS, std::string RRHS) override;
    void emit_mul(std::string ROUT, std::string RLHS, std::string RRHS) override;
    void emit_div(std::string ROUT, std::string RLHS, std::string RRHS) override;

    void emit_move(std::string rdest, std::string rsrc) override;

    void emit_reserve_stack_space(uint64_t size) override;
    void emit_store_from_stack(uint64_t align, uint64_t size, std::string rsrc) override;

    std::string emit_load_from_stack(uint64_t align, uint64_t size, std::string load_reg = "") override;
    std::string emit_loadaddr_from_stack(uint64_t align, std::string load_reg = "") override;

    void emit_call(std::string name) override;

    void emit_push(std::string reg) override;
    void emit_pop(std::string reg) override;

    void emit_single_ret() override;
    void emit_label(std::string name) override;
  };
} // namespace hcc
