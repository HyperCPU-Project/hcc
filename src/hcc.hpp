#pragma once
#include <backend/backend.hpp>
#include <flags.hpp>
#include <ir/ir.hpp>
#include <pch.hpp>
#include <result.hpp>
#include <yy.hpp>

#ifdef HCC_NOPRIVATE
#define hccprivate public:
#else
#define hccprivate private:
#endif

extern std::string hcc_compile_error;

namespace hcc {
  class Value;

  class HCC {
    hccprivate Parser* parser;
    FILE* outfd;

  public:
    std::vector<std::string> sources;
    bool print_ast;
    std::string asm_output;

    Backend* backend;
    IR ir;

    enum Optimization { OPT_CONSTANT_FOLDING,
                        OPT_FUNCTION_BODY_ELIMINATION,
                        OPT_DCE,
                        OPT_FP_OMISSION,
                        OPT_STACK_RESERVE,
                        OPT_CONSTANT_PROPAGATION };
    Flags<Optimization> optimizations;

    FunctionMetadata current_function;
    std::stack<std::unique_ptr<Value>> values;

    HCC();
    ~HCC();

    Result<NoSuccess, std::string> parseAndCompile();

    void openOutput(std::string filename);

    Result<NoSuccess, std::string> selectBackend(std::string name);

    Optimization getOptimizationFromName(std::string name);

    FILE* getOutFd();
  };

} // namespace hcc
