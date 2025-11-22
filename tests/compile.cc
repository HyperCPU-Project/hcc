#include "compile.hpp"
#include <cstd_pch.hpp>
#include <string>
#include <util.hpp>

tl::expected<std::string, std::string> compileQuick(std::string code, std::string backend) {
  hcc::HCC hcc;
  hcc.print_ast = true;
  hcc.source = code;

  fmt::print("[hcctest] compiling temp C\n");
  fmt::print("[hcctest] code:\n");
  fmt::print("{}\n", code);

  {
    auto result = hcc.SelectBackend(backend);
    if (!result.has_value())
      return tl::unexpected(result.error());
  }

  {
    auto result = hcc.ParseAndCompile();
    if (!result.has_value())
      return tl::unexpected(result.error());

    return hcc.backend->output;
  }
}
