#include <ast/ast.hpp>
#include <hcc.hpp>
#include <pch.hpp>
#include <util.hpp>
#include <yy.hpp>

using namespace hcc;

int main(int argc, char** argv) {
  HCC hcc;

  hcc.openOutput("a.s");
  hcc.selectBackend("hypercpu");

  argsShift();
  for ([[maybe_unused]] int i = 0; argc; ++i) {
    std::string arg = argsShift();

    if (arg == "--help" || arg == "-h") {
      fmt::print(R"(usage: hcc [OPTION]... [INPUT]
options:
  --help | -h      print this message
  -o               set output filename
  --backend        set a backend
  --ast            print AST
  -f               enable optimizations
  -fno-            disable optimizations
backends:
  qproc
  hypercpu (beta)
)");
      return 0;
    } else if (arg == "-o") {
      hcc.openOutput(argsShift());
    } else if (arg == "--ast") {
      hcc.print_ast = true;
    } else if (arg == "--backend") {
      auto result = hcc.selectBackend(argsShift());
      if (result.is_error()) {
        fmt::print("[hcc] failed to select a backend: {}\n", result.get_error().value());
        return 1;
      }
    } else if (arg.starts_with("-fno-")) {
      std::string optimization_name = arg;
      optimization_name.erase(optimization_name.begin(), optimization_name.begin() + 5);

      std::optional<Optimization> optimization_opt = hcc.getOptimizationFromName(optimization_name);
      if (!optimization_opt.has_value()) {
        fmt::print("[hcc] no such optimization: {}\n", optimization_name);
        return 1;
      } else {
        Optimization optimization = optimization_opt.value();
        if (hcc.optimizations.HasFlag(optimization))
          hcc.optimizations.UnsetFlag(optimization);
      }
    } else if (arg.starts_with("-f")) {
      std::string optimization_name = arg;
      optimization_name.erase(optimization_name.begin(), optimization_name.begin() + 2);

      std::optional<Optimization> optimization_opt = hcc.getOptimizationFromName(optimization_name);
      if (!optimization_opt.has_value()) {
        fmt::print("[hcc] no such optimization: {}\n", optimization_name);
        return 1;
      } else {
        Optimization optimization = optimization_opt.value();
        hcc.optimizations.SetFlag(optimization);
      }
    } else if (arg.starts_with("-")) {
      fmt::print("[hcc] unknown flag: {}\n", arg);
      return 1;
    } else {
      hcc.sources.push_back(arg);
    }
  }

  {
    auto result = hcc.parseAndCompile();
    if (result.is_error()) {
      fmt::print("[hcc] error: {}\n", result.get_error().value());
    }
  }

  return 0;
}
