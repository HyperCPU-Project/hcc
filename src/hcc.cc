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

HCC::HCC() : outfd(nullptr), print_ast(false), backend(nullptr), values() {
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

Result<NoSuccess, std::string> HCC::parseAndCompile() {
  yyscan_t scanner;
  YY_BUFFER_STATE buffer;

  yylex_init(&scanner);
  yyset_extra(this->parser, scanner);

  hcc_parse_error.clear();
  hcc_compile_error.clear();

  if (sources.empty()) {
    return Result<NoSuccess, std::string>::error("no sources provided");
  }

  if (!outfd) {
    return Result<NoSuccess, std::string>::error("outfd == nullptr");
  }

  if (!backend) {
    return Result<NoSuccess, std::string>::error("no backend selected");
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
    return Result<NoSuccess, std::string>::error(fmt::format("root == nullptr (parse error: {})", hcc_parse_error));
  }

  if (print_ast) {
    parser->root->print();
  }

  if (!parser->root->compile(this)) {
    return Result<NoSuccess, std::string>::error("compile error: " + hcc_compile_error);
  }

  if (ir.results_in_error(this)) {
    return Result<NoSuccess, std::string>::error("ir compile error: " + hcc_compile_error);
  }
  ir.performStaticOptimizations(this);
  if (!ir.compile(this)) {
    return Result<NoSuccess, std::string>::error("ir compile error: " + hcc_compile_error);
  }

  fmt::fprintf(outfd, "%s", backend->output);

  if (parser->root)
    delete parser->root;

  return Result<NoSuccess, std::string>::success({});
}

void HCC::openOutput(std::string filename) {
  if (outfd)
    fclose(outfd);
  outfd = fopen(filename.c_str(), "w");
}

Result<NoSuccess, std::string> HCC::selectBackend(std::string name) {
  if (backend)
    delete backend;

  if (name == "qproc") {
    backend = new QprocBackend();
  } else if (name == "hypercpu") {
    backend = new HyperCPUBackend();
  } else {
    return Result<NoSuccess, std::string>::error("no such backend");
  }

  return Result<NoSuccess, std::string>::success({});
}

HCC::Optimization HCC::getOptimizationFromName(std::string name) {
  if (name == "constant-folding") {
    return OPT_CONSTANT_FOLDING;
  } else if (name == "omit-frame-pointer") {
    return OPT_FP_OMISSION;
  } else if (name == "function-body-elimination") {
    return OPT_FUNCTION_BODY_ELIMINATION;
  } else if (name == "dce") {
    return OPT_DCE;
  } else if (name == "stack-reserve") {
    return OPT_STACK_RESERVE;
  } else if (name == "constant-propagation") {
    return OPT_CONSTANT_PROPAGATION;
  }

  return (Optimization)-1;
}

FILE* HCC::getOutFd() {
  return outfd;
}
