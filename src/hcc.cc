#include <backend/hypercpu/hypercpu_backend.hpp>
#include <backend/qproc/qproc_backend.hpp>
#include <dep_pch.hpp>
#include <hcc.hpp>
#include <util.hpp>
#include <value/value.hpp>

using namespace hcc;

std::string hcc_compile_error = "";

HCC::HCC() : print_ast(false), backend(nullptr), values() {
  current_function.align = 0;
  optimizations.SetFlag(Optimization::OPT_CONSTANT_FOLDING);
  optimizations.SetFlag(Optimization::OPT_FUNCTION_BODY_ELIMINATION);
  optimizations.SetFlag(Optimization::OPT_DCE);
  optimizations.SetFlag(Optimization::OPT_FP_OMISSION);
  optimizations.SetFlag(Optimization::OPT_STACK_RESERVE);
  optimizations.SetFlag(Optimization::OPT_CONSTANT_PROPAGATION);
}

tl::expected<void, std::string> HCC::ParseAndCompile() {
  hcc_compile_error.clear();

  if (sources.empty()) {
    return tl::unexpected<std::string>("no sources provided");
  }

  if (!outfd) {
    return tl::unexpected<std::string>("outFd == nullptr");
  }

  if (!backend) {
    return tl::unexpected<std::string>("no backend selected");
  }

  std::string code = "";
  for (std::string source : sources) {
    auto result = ReadFile(source);
    if (!result.has_value()) {
      fmt::print("[hcc] failed to read {}: {}\n", source, result.value());
    }

    code += result.value() + "\n";
  }

  throw std::runtime_error("unimplemented");

  /*
  buffer = yy_scan_string(code.c_str(), scanner);
  yyparse(scanner, this->parser);

  yy_delete_buffer(buffer, scanner);
  yylex_destroy(scanner);

  if (!parser->root) {
    return tl::unexpected<std::string>("root == nullptr (parse error: {})");
  }

  if (print_ast) {
    parser->root->Print();
  }

  if (!parser->root->Compile(this)) {
    return tl::unexpected<std::string>("compile error: " + hcc_compile_error);
  }

  if (ir.resultsInError(this)) {
    return tl::unexpected<std::string>("ir compile error: " +
                                       hcc_compile_error);
  }
  ir.PerformStaticOptimizations(this);
  if (!ir.compile(this)) {
    return tl::unexpected<std::string>("ir compile error: " +
                                       hcc_compile_error);
  }

  outfd << backend->output;

  if (parser->root)
    delete parser->root;
    */

  return {};
}

void HCC::OpenOutput(std::string filename) {
  if (outfd.is_open()) {
    outfd.close();
  }
  outfd = std::ofstream(filename);
}

tl::expected<void, std::string> HCC::SelectBackend(std::string name) {
  if (name == "qproc") {
    backend = std::make_shared<QprocBackend>();
  } else if (name == "hypercpu") {
    backend = std::make_shared<HyperCPUBackend>();
  } else {
    return tl::unexpected<std::string>("no such backend");
  }

  return {};
}

std::optional<Optimization> HCC::GetOptimizationFromName(std::string name) {
  auto names = mapbox::eternal::map<mapbox::eternal::string, Optimization>(
      {{"constant-folding", Optimization::OPT_CONSTANT_FOLDING},
       {"emit-frame-pointer", Optimization::OPT_FP_OMISSION},
       {"function-body-elimination",
        Optimization::OPT_FUNCTION_BODY_ELIMINATION},
       {"dce", Optimization::OPT_DCE},
       {"stack-reserve", Optimization::OPT_STACK_RESERVE},
       {"constant-propagation", Optimization::OPT_CONSTANT_PROPAGATION}});

  if (!names.contains(name.c_str()))
    return {};

  return names.at(name.c_str());
}

std::ofstream& HCC::GetOutFd() {
  return outfd;
}
