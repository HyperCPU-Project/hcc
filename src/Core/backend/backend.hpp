#pragma once

#include <metadata.hpp>
#include <pch.hpp>

namespace hcc {
  class Backend {
  protected:
    uint64_t reg_index;

  public:
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

    std::string output;
    ABIMetadata abi;

    bool codegen_comments = false;

    std::map<std::string, TypeMetadata> types;

    Backend();
    virtual ~Backend() = 0;

    virtual void PeepholeOptimize();

    virtual std::string CompileCalls();

    virtual void EmitFunctionPrologue(std::string name);
    virtual void EmitFunctionEpilogue();

    virtual std::string EmitMovConst(uint64_t value, std::string reg_name = "");

    virtual void EmitAdd(std::string ROUT, std::string RLHS, std::string RRHS);
    virtual void EmitSub(std::string ROUT, std::string RLHS, std::string RRHS);
    virtual void EmitMul(std::string ROUT, std::string RLHS, std::string RRHS);
    virtual void EmitDiv(std::string ROUT, std::string RLHS, std::string RRHS);

    virtual void EmitMove(std::string rdest, std::string rsrc);

    virtual void EmitReserveStackSpace(uint64_t size);

    virtual std::string EmitLoadFromStack(uint64_t align, uint64_t size, std::string load_reg = "");
    virtual void EmitStoreToStack(uint64_t align, uint64_t size, std::string rsrc);
    virtual std::string EmitLoadaddrFromStack(uint64_t align, std::string load_reg = "");

    virtual void EmitCall(std::string name);

    virtual void EmitPush(std::string reg);
    virtual void EmitPushImm(long val);
    virtual void EmitPop(std::string reg);

    virtual void EmitSingleRet();
    virtual void EmitLabel(std::string name);

    virtual TypeMetadata* GetTypeFromName(std::string name);
  };
} // namespace hcc
