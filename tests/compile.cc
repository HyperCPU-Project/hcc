#include "compile.hpp"
#include "main.hpp"
#include <cstd_pch.hpp>
#include <cstdio>
#include <filesystem>
#include <string>
#include <util.hpp>

std::string compile_output;

tl::expected<void, std::string> compileQuick(std::string code, std::string backend) {
  compile_output.clear();

  hcc::HCC hcc;
  hcc.print_ast = true;

  std::filesystem::create_directory("tests_tmp");

  hcc.source = code;
  hcc.OpenOutput("tests_tmp/a.out");

  fmt::print("[hcctest] compiling temp C\n");
  fmt::print("[hcctest] code:\n");
  fmt::print("{}\n", code);

  {
    auto result = hcc.SelectBackend(backend);
    if (!result.has_value())
      return result;
  }

  {
    auto result = hcc.ParseAndCompile();
    if (!result.has_value())
      return result;

    compile_output = ReadFile("tests_tmp/a.out").value();
    compile_output = hcc.backend->output;

    return result;
  }
}
