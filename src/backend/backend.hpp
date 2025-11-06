#pragma once

#include <metadata.hpp>
#include <pch.hpp>

namespace hcc {
  class Backend {
  protected:
    uint64_t reg_index;

  public:
    ABIMetadata abi;

    bool codegen_comments = false;

    std::map<std::string, TypeMetadata> types;

    Backend();
    virtual ~Backend() = 0;

    uint64_t increment_reg_index();
    void reset_reg_index();

    struct EmitCall {
      enum {
        FUNCTION_PROLOGUE,
        FUNCTION_EPILOGUE,
        MOV_CONST,
        ADD,
        SUB,
        MUL,
        DIV,
        MOVE,
        RESERVE_STACK_SPACE,
        LOAD_FROM_STACK,
        STORE_FROM_STACK,
        LOADADDR_FROM_STACK,
        CALL,
        PUSH,
        PUSH_IMM,
        POP,
        SINGLE_RET,
        LABEL,
      } type;

      struct {
        std::string name;
      } function_prologue;

      struct {
        uint64_t value;
        std::string reg_name;
      } mov_const;

      struct {
        std::string ROUT, RLHS, RRHS;
      } add;

      struct {
        std::string ROUT, RLHS, RRHS;
      } sub;

      struct {
        std::string ROUT, RLHS, RRHS;
      } mul;

      struct {
        std::string ROUT, RLHS, RRHS;
      } div;

      struct {
        std::string rdest, rsrc;
      } move;

      struct {
        uint64_t size;
      } reserve_stack_space;

      struct {
        uint64_t align, size;
        std::string load_reg;
      } load_from_stack;

      struct {
        uint64_t align, size;
        std::string rsrc;
      } store_from_stack;

      struct {
        uint64_t align;
        std::string load_reg;
      } loadaddr_from_stack;

      struct {
        std::string name;
      } call;

      struct {
        std::string reg;
      } push;

      struct {
        long val;
      } push_imm;

      struct {
        std::string reg;
      } pop;

      struct {
        std::string name;
      } label;
    };

    std::vector<EmitCall> emitcalls;
    virtual void peephole_optimize();
    virtual std::string compile_calls();

    virtual void emit_function_prologue(std::string name);
    virtual void emit_function_epilogue();

    virtual std::string emit_mov_const(uint64_t value, std::string reg_name = "");

    virtual void emit_add(std::string ROUT, std::string RLHS, std::string RRHS);
    virtual void emit_sub(std::string ROUT, std::string RLHS, std::string RRHS);
    virtual void emit_mul(std::string ROUT, std::string RLHS, std::string RRHS);
    virtual void emit_div(std::string ROUT, std::string RLHS, std::string RRHS);

    virtual void emit_move(std::string rdest, std::string rsrc);

    virtual void emit_reserve_stack_space(uint64_t size);

    virtual std::string emit_load_from_stack(uint64_t align, uint64_t size, std::string load_reg = "");
    virtual void emit_store_from_stack(uint64_t align, uint64_t size, std::string rsrc);
    virtual std::string emit_loadaddr_from_stack(uint64_t align, std::string load_reg = "");

    virtual void emit_call(std::string name);

    virtual void emit_push(std::string reg);
    virtual void emit_push_imm(long val);
    virtual void emit_pop(std::string reg);

    virtual void emit_single_ret();
    virtual void emit_label(std::string name);

    virtual TypeMetadata* get_type_from_name(std::string name);
  };
} // namespace hcc
