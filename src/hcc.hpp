#pragma once
#include <backend/backend.hpp>
#include <cstd_pch.hpp>
#include <dep_pch.hpp>
#include <flags.hpp>
#include <function_metadata.hpp>
#include <ir/ir.hpp>
#include <optimization.hpp>

#ifdef HCC_NOPRIVATE
#define hccprivate public:
#else
#define hccprivate private:
#endif

extern std::string hcc_compile_error;

namespace hcc {
  class Value;

  class HCC {
    std::ofstream outfd;

  public:
    std::vector<std::string> sources;
    bool print_ast;

    std::shared_ptr<Backend> backend;
    IR ir;

    Flags<Optimization> optimizations;

    FunctionMetadata current_function;
    std::stack<std::unique_ptr<Value>> values;

    HCC();

    tl::expected<void, std::string> ParseAndCompile();

    void OpenOutput(std::string filename);

    tl::expected<void, std::string> SelectBackend(std::string name);

    std::optional<Optimization> GetOptimizationFromName(std::string name);

    std::ofstream& GetOutFd();
  };

} // namespace hcc
