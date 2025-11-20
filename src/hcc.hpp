#pragma once
#include "cstd_pch.hpp"
#include "dep_pch.hpp"
#include "driver/driver.hpp"
#include "lexer/lexer.hpp"
#include <backend/backend.hpp>
#include <flags.hpp>
#include <function_metadata.hpp>
#include <ir/ir.hpp>
#include <optimization.hpp>

namespace hcc {
  class Value;
  class Parser;

  class HCC {
  private:
    Driver driver;
    Parser* parser; // This is a pointer because we can't include parser.tab.hpp. The reason why we can't include this is because other sources may not have include dirs setup properly (with xmake, you just can't even include generated dirs!)
    std::ofstream outfd;

    std::string compile_error;

  public:
    std::string source;
    bool print_ast;

    std::shared_ptr<Backend> backend;
    IR ir;

    Flags<Optimization> optimizations;

    FunctionMetadata current_function;
    std::stack<std::unique_ptr<Value>> values;

    HCC();
    ~HCC();

    tl::expected<void, std::string> ParseAndCompile();

    void OpenOutput(std::string filename);

    tl::expected<void, std::string> SelectBackend(std::string name);

    std::optional<Optimization> GetOptimizationFromName(std::string name);

    std::ofstream& GetOutFd();

    friend class Backend;
    friend class IR;
  };
} // namespace hcc
