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
    FILE* outfd;

  public:
    std::vector<std::string> sources;
    bool print_ast;

    Backend* backend;
    IR ir;

    Flags<Optimization> optimizations;

    FunctionMetadata current_function;
    std::stack<std::unique_ptr<Value>> values;

    HCC();
    ~HCC();

    Result<void, std::string> parseAndCompile();

    void openOutput(std::string filename);

    Result<void, std::string> selectBackend(std::string name);

    std::optional<Optimization> getOptimizationFromName(std::string name);

    FILE* getOutFd();
  };

} // namespace hcc
