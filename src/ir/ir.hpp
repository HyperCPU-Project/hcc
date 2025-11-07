#pragma once
#include <metadata.hpp>
#include <pch.hpp>

namespace hcc {
  class Backend;
  class HCC;

  struct IrOpcode {
    enum {
      IR_NULL,
      IR_END,
      IR_FUNCDEF,
      IR_CREG, // create register value
      IR_CCTV, // create compile time value
      IR_CSV,  // create stack value
      IR_RET,
      IR_ALLOCA,
      IR_ADD,
      IR_SUB,
      IR_MUL,
      IR_DIV,
      IR_ASSIGN,
      IR_ASM,
      IR_VARREF,
      IR_ADDROF,
      IR_CALL,
      IR_LINE,  // just marker that does nothing, needed for static optimizations
      IR_RESET, // resets register counter
      IR_RESERVE,
    } type;

    struct {
      std::string name;

      std::vector<TypeMetadata> argTypes;
      std::vector<std::string> argNames;

      bool needStack = false;
    } funcdef;

    struct {
      uint64_t value;
      std::string regName = "";
    } creg;

    struct {
      uint64_t value;
    } cctv;

    struct {
      TypeMetadata md;
    } csv;

    struct {
      TypeMetadata md;
      std::string name;
    } alloca;

    struct {
      std::string name;
    } assign;

    struct {
      std::string code;
    } asm_;

    struct {
      std::string name;
    } varref;

    struct {
      std::string name;
    } addrof;

    struct {
      std::string name;
    } call;

    struct {
      uint64_t bytes;
    } reserve;
  };

  class IR {
  private:
    unsigned long index;
    std::vector<IrOpcode> ir;

    IrOpcode next();
    IrOpcode peek(unsigned long count = 1);

    void optimizeDceUnused(HCC* hcc);
    void optimizeStackSetup(HCC* hcc);
    void optimizeStackReserve(HCC* hcc);
    void optimizeConstantPropagation(HCC* hcc);

  public:
    size_t passesForEachOptimization = 64;

    IR();

    void addLine();
    void addReset();
    void add(IrOpcode op);

    bool opcodeAffectsStack(IrOpcode op);

    void performStaticOptimizations(HCC* hcc);
    bool compile(HCC* hcc);

    bool resultsInError(HCC* hcc);
  };
} // namespace hcc
