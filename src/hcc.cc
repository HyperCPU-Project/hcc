#include "dep_pch.hpp"
#include <backend/hypercpu/hypercpu_backend.hpp>
#include <backend/qproc/qproc_backend.hpp>
#include <hcc.hpp>
#include <parser.tab.hpp>
#include <util.hpp>
#include <value/value.hpp>

using namespace hcc;

HCC::HCC()
    : driver(), parser(new Parser(driver)), print_ast(false), backend(nullptr), values() {

  current_function.align = 0;
  optimizations.SetFlag(Optimization::OPT_CONSTANT_FOLDING);
  optimizations.SetFlag(Optimization::OPT_FUNCTION_BODY_ELIMINATION);
  optimizations.SetFlag(Optimization::OPT_DCE);
  optimizations.SetFlag(Optimization::OPT_FP_OMISSION);
  optimizations.SetFlag(Optimization::OPT_STACK_RESERVE);
  optimizations.SetFlag(Optimization::OPT_CONSTANT_PROPAGATION);
}

HCC::~HCC() {
  if (parser)
    delete parser;
}

tl::expected<void, std::string> HCC::ParseAndCompile() {
  compile_error.clear();

  if (!backend) {
    return tl::unexpected<std::string>("no backend selected");
  }

  Lexer lexer(source);
  {
    auto result = lexer.Tokenize();
    if (!result.has_value()) {
      throw std::runtime_error("tokenize error: " + result.error());
    }

    driver.tokens = result.value();
  }

  parser->parse();

  if (driver.error.has_value()) {
    return tl::unexpected<std::string>("parse error: " + driver.error.value());
  }

  if (print_ast) {
    driver.root->Print();
  }

  if (!driver.root->Compile(this)) {
    return tl::unexpected<std::string>("compile error: " + compile_error);
  }

  if (ir.ResultsInError(this)) {
    return tl::unexpected<std::string>("ir compile error: " + compile_error);
  }
  ir.PerformStaticOptimizations(this);
  if (!ir.Compile(this)) {
    return tl::unexpected<std::string>("ir compile error: " + compile_error);
  }

  if (outfd.is_open())
    outfd << backend->output;

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
    backend = std::make_shared<QprocBackend>(this);
  } else if (name == "hypercpu") {
    backend = std::make_shared<HyperCPUBackend>(this);
  } else {
    return tl::unexpected<std::string>("no such backend");
  }

  return {};
}

std::optional<Optimization> HCC::GetOptimizationFromName(std::string name) {
  auto names = mapbox::eternal::map<mapbox::eternal::string, Optimization>({{"constant-folding", Optimization::OPT_CONSTANT_FOLDING},
                                                                            {"emit-frame-pointer", Optimization::OPT_FP_OMISSION},
                                                                            {"function-body-elimination", Optimization::OPT_FUNCTION_BODY_ELIMINATION},
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
