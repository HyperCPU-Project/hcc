#include "compile.hpp"
#include "main.hpp"
#include <cstdio>
#include <filesystem>
#include <pch.hpp>
#include <string>
#include <util.hpp>

std::string compile_output = "";

std::optional<std::string> compileQuick(std::string code, std::string backend) {
  compile_output.clear();

  hcc::HCC hcc;

  std::filesystem::create_directory("tests_tmp");
  std::string filename = fmt::format("tests_tmp/test{}.c", ++test_counter);

  std::ofstream file;
  file.open(filename);
  file << code;
  file.close();

  hcc.sources.push_back(filename);

  hcc.out_fd = fopen("tests_tmp/a.out", "w");

  fmt::print("[hcctest] compiling temp C file {}\n", filename);

  {
    auto result = hcc.SelectBackend(backend);
    if (result.has_value())
      return result.value();
  }

  {
    auto result = hcc.ParseAndCompile();
    if (result.has_value())
      return result.value();

    compile_output = ReadFile("tests_tmp/a.out").value();
    compile_output = hcc.backend->output;
    /*
    fmt::println("-------");
    fmt::println("{}", hcc.backend->output);
    fmt::println("-------");
    */

    return {};
  }
}
