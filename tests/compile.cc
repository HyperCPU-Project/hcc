#include "compile.hpp"
#include "pch.hpp"
#include <string>
#include <util.hpp>

std::string compile_output = "";

std::optional<std::string> compileQuick(std::string code, std::string backend) {
  compile_output.clear();

  hcc::HCC hcc;

  hcc.source = code;
  hcc.OpenOutput("a.out");

  fmt::print("[hcctest] compiling...\n");

  {
    auto result = hcc.SelectBackend(backend);
    if (result.has_value())
      return result.value();
  }

  {
    auto result = hcc.ParseAndCompile();
    if (result->starts_with("failed to read")) {
      EXPECT_EQ(std::string("unexpected read fail"), std::string());
    }
    if (result.has_value())
      return result.value();

    compile_output = ReadFile("tests_tmp/a.out").value();
    compile_output = hcc.backend->output;

    return {};
  }
}
