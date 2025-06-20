#include <backend/hypercpu/hypercpu_backend.hpp>
#include <backend/qproc/qproc_backend.hpp>
#include <fmt/printf.h>
#include <hcc.hpp>
#include <parser.hpp>
#include <util.hpp>
#include <value/value.hpp>
#include <yy.hpp>

using namespace hcc;

typedef void* yyscan_t;

void yylex_init(yyscan_t*);
void yylex_destroy(yyscan_t);
void yyset_extra(hcc::Parser* user_defined, void* yyscanner);

std::string hcc_compile_error = "";

HCC::HCC()
    : outfd(nullptr), print_ast(false), backend(nullptr), values() {
  parser = new Parser();

  current_function.align = 0;
  optimizations.SetFlag(OPT_CONSTANT_FOLDING);
  optimizations.SetFlag(OPT_FUNCTION_BODY_ELIMINATION);
  optimizations.SetFlag(OPT_DCE);
  optimizations.SetFlag(OPT_FP_OMISSION);
  optimizations.SetFlag(OPT_STACK_RESERVE);
  optimizations.SetFlag(OPT_CONSTANT_PROPAGATION);
}

HCC::~HCC() {
  if (outfd)
    fclose(outfd);
  if (backend)
    delete backend;
}

Result<void, std::string> HCC::parseAndCompile() {
  yyscan_t scanner;
  YY_BUFFER_STATE buffer;

  yylex_init(&scanner);
  yyset_extra(this->parser, scanner);

  hcc_parse_error.clear();
  hcc_compile_error.clear();

  if (sources.empty()) {
    return Result<void, std::string>::error("no sources provided");
  }

  if (!outfd) {
    return Result<void, std::string>::error("outfd == nullptr");
  }

  if (!backend) {
    return Result<void, std::string>::error("no backend selected");
  }

  std::string code = "";
  for (std::string source : sources) {
    auto result = readFile(source);
    if (result.is_error()) {
      fmt::print("[hcc] failed to read {}: {}\n", source, result.get_error().value());
    }

    code += result.get_success().value() + "\n";
  }

  buffer = yy_scan_string(code.c_str(), scanner);
  yyparse(scanner, this->parser);

  yy_delete_buffer(buffer, scanner);
  yylex_destroy(scanner);

  if (!parser->root) {
    return Result<void, std::string>::error(fmt::format("root == nullptr (parse error: {})", hcc_parse_error));
  }

  if (print_ast) {
    parser->root->print();
  }

  if (!parser->root->compile(this)) {
    return Result<void, std::string>::error("compile error: " + hcc_compile_error);
  }

  if (ir.results_in_error(this)) {
    return Result<void, std::string>::error("ir compile error: " + hcc_compile_error);
  }
  ir.performStaticOptimizations(this);
  if (!ir.compile(this)) {
    return Result<void, std::string>::error("ir compile error: " + hcc_compile_error);
  }

  backend->peephole_optimize();
  asm_output = backend->compile_calls();

  fmt::fprintf(outfd, "%s", asm_output);

  if (parser->root)
    delete parser->root;

  return Result<void, std::string>::success();
}

void HCC::openOutput(std::string filename) {
  if (outfd)
    fclose(outfd);
  outfd = fopen(filename.c_str(), "w");
}

Result<void, std::string> HCC::selectBackend(std::string name) {
  if (backend)
    delete backend;

  if (name == "qproc") {
    backend = new QprocBackend();
  } else if (name == "hypercpu") {
    backend = new HyperCPUBackend();
  } else {
    return Result<void, std::string>::error("no such backend");
  }

  return Result<void, std::string>::success();
}

HCC::Optimization HCC::getOptimizationFromName(std::string name) {
  auto names = mapbox::eternal::map<mapbox::eternal::string, HCC::Optimization>({{"constant-folding", OPT_CONSTANT_FOLDING},
                                                                                 {"emit-frame-pointer", OPT_FP_OMISSION},
                                                                                 {"function-body-elimination", OPT_FUNCTION_BODY_ELIMINATION},
                                                                                 {"dce", OPT_DCE},
                                                                                 {"stack-reserve", OPT_STACK_RESERVE},
                                                                                 {"constant-propagation", OPT_CONSTANT_PROPAGATION}});

  if (!names.contains(name.c_str()))
    return (Optimization)-1;

  return names.at(name.c_str());
}

FILE* HCC::getOutFd() {
  return outfd;
}
