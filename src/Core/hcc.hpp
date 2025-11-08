#pragma once
#include <backend/backend.hpp>
#include <flags.hpp>
#include <ir/ir.hpp>
#include <optimization.hpp>
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
    FILE* out_fd;

  public:
    std::vector<std::string> sources;
    bool print_ast;
    std::string asm_output;

    Backend* backend;
    IR ir;

    Flags<Optimization> optimizations;

    FunctionMetadata current_function;

    HCC();
    ~HCC();

    Result<void, std::string> ParseAndCompile();

    void OpenOutput(std::string filename);

    Result<void, std::string> SelectBackend(std::string name);

    std::optional<Optimization> GetOptimizationFromName(std::string name);

    FILE* GetOutFd();
  };

} // namespace hcc
