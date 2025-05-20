#include <compile.hpp>
#include <cstdio>
#include <filesystem>
#include <main.hpp>
#include <pch.hpp>
#include <string>
#include <util.hpp>

std::string compile_output = "";
char compile_output_raw[32767];

Result<NoSuccess, std::string> compile_quick(std::string code, std::string backend) {
  compile_output.clear();

  hcc::HCC hcc;

  std::filesystem::create_directory("tests_tmp");
  std::string filename = fmt::format("tests_tmp/test{}.c", ++test_counter);

  std::ofstream file;
  file.open(filename);
  file << code;
  file.close();

  hcc.sources.push_back(filename);

  hcc.outfd = fmemopen(compile_output_raw, sizeof(compile_output_raw), "w");

  fmt::print("[hcctest] compiling temp C file {}\n", filename);

  auto result = hcc.selectBackend(backend);
  if (result.is_error())
    return result;

  result = hcc.parseAndCompile();
  compile_output = hcc.backend->output;

  return result;
}
