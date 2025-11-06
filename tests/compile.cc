#include "compile.hpp"
#include "main.hpp"
#include <cstdio>
#include <filesystem>
#include <pch.hpp>
#include <string>
#include <util.hpp>

std::string compile_output = "";

Result<void, std::string> compile_quick(std::string code, std::string backend) {
  compile_output.clear();

  hcc::HCC hcc;

  std::filesystem::create_directory("tests_tmp");
  std::string filename = fmt::format("tests_tmp/test{}.c", ++test_counter);

  std::ofstream file;
  file.open(filename);
  file << code;
  file.close();

  hcc.sources.push_back(filename);

  hcc.outfd = fopen("tests_tmp/a.out", "w");

  fmt::print("[hcctest] compiling temp C file {}\n", filename);

  {
    auto result = hcc.selectBackend(backend);
    if (result.is_error())
      return result;
  }

  {
    auto result = hcc.parseAndCompile();
    if (result.is_error())
      return result;

    compile_output = readFile("tests_tmp/a.out").get_success().value();
    compile_output = hcc.backend->output;
    /*
    fmt::println("-------");
    fmt::println("{}", hcc.backend->output);
    fmt::println("-------");
    */

    return result;
  }
}
