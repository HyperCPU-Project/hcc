#include <backend/hypercpu/hypercpu_backend.hpp>
#include <backend/qproc/qproc_backend.hpp>
#include <fmt/printf.h>
#include <hcc.hpp>
#include <parser.tab.hpp>
#include <util.hpp>
#include <yy.hpp>

using namespace hcc;

typedef void* yyscan_t;

void yylex_init(yyscan_t*);
void yylex_destroy(yyscan_t);
void yyset_extra(hcc::Parser* user_defined, void* yyscanner);

std::string hcc_compile_error = "";

HCC::HCC()
    : out_fd(nullptr), print_ast(false), backend(nullptr) {
  parser = new Parser();

  current_function.align = 0;
  optimizations.SetFlag(Optimization::OPT_CONSTANT_FOLDING);
  optimizations.SetFlag(Optimization::OPT_FUNCTION_BODY_ELIMINATION);
  optimizations.SetFlag(Optimization::OPT_DCE);
  optimizations.SetFlag(Optimization::OPT_FP_OMISSION);
  optimizations.SetFlag(Optimization::OPT_STACK_RESERVE);
  optimizations.SetFlag(Optimization::OPT_CONSTANT_PROPAGATION);
}

HCC::~HCC() {
  if (out_fd)
    fclose(out_fd);
  if (backend)
    delete backend;
}

Result<void, std::string> HCC::ParseAndCompile() {
  yyscan_t scanner;
  YY_BUFFER_STATE buffer;

  yylex_init(&scanner);
  yyset_extra(this->parser, scanner);

  hcc_parse_error.clear();
  hcc_compile_error.clear();

  if (sources.empty()) {
    return Result<void, std::string>::Error("no sources provided");
  }

  if (!out_fd) {
    return Result<void, std::string>::Error("outFd == nullptr");
  }

  if (!backend) {
    return Result<void, std::string>::Error("no backend selected");
  }

  std::string code = "";
  for (std::string source : sources) {
    auto result = ReadFile(source);
    if (result.IsError()) {
      fmt::print("[hcc] failed to read {}: {}\n", source, result.GetError().value());
    }

    code += result.GetSuccess().value() + "\n";
  }

  buffer = yy_scan_string(code.c_str(), scanner);
  yyparse(scanner, this->parser);

  yy_delete_buffer(buffer, scanner);
  yylex_destroy(scanner);

  if (!parser->root) {
    return Result<void, std::string>::Error(fmt::format("root == nullptr (parse error: {})", hcc_parse_error));
  }

  if (print_ast) {
    parser->root->Print();
  }

  if (!parser->root->Compile(this)) {
    return Result<void, std::string>::Error("compile error: " + hcc_compile_error);
  }

  if (ir.resultsInError(this)) {
    return Result<void, std::string>::Error("ir compile error: " + hcc_compile_error);
  }
  ir.PerformStaticOptimizations(this);
  if (!ir.compile(this)) {
    return Result<void, std::string>::Error("ir compile error: " + hcc_compile_error);
  }

  backend->peephole_optimize();
  asm_output = backend->CompileCalls();

  fmt::fprintf(out_fd, "%s", asm_output);

  if (parser->root)
    delete parser->root;

  return Result<void, std::string>::Success();
}

void HCC::OpenOutput(std::string filename) {
  if (out_fd)
    fclose(out_fd);
  out_fd = fopen(filename.c_str(), "w");
}

Result<void, std::string> HCC::SelectBackend(std::string name) {
  if (backend)
    delete backend;

  if (name == "qproc") {
    backend = new QprocBackend();
  } else if (name == "hypercpu") {
    backend = new HyperCPUBackend();
  } else {
    return Result<void, std::string>::Error("no such backend");
  }

  return Result<void, std::string>::Success();
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

FILE* HCC::GetOutFd() {
  return out_fd;
}
