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

  std::filesystem::create_directory("tests_tmp");
  std::string filename = fmt::format("tests_tmp/test{}.c", ++test_counter);

  std::ofstream file;
  file.open(filename);
  file << code;
  file.close();

  hcc.source = code;
  hcc.OpenOutput("tests_tmp/a.out");

  fmt::print("[hcctest] compiling temp C file {}\n", filename);

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
