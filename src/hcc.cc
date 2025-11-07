#include <backend/hypercpu/hypercpu_backend.hpp>
#include <backend/qproc/qproc_backend.hpp>
#include <fmt/printf.h>
#include <hcc.hpp>
#include <parser.tab.hpp>
#include <util.hpp>
#include <value/value.hpp>
#include <yy.hpp>

using namespace hcc;

typedef void* yyscan_t;

void yylex_init(yyscan_t*);
void yylex_destroy(yyscan_t);
void yyset_extra(hcc::Parser* user_defined, void* yyscanner);

std::string hccCompileError = "";

HCC::HCC()
    : outFd(nullptr), printAst(false), backend(nullptr), values() {
  parser = new Parser();

  currentFunction.align = 0;
  optimizations.setFlag(OPT_CONSTANT_FOLDING);
  optimizations.setFlag(OPT_FUNCTION_BODY_ELIMINATION);
  optimizations.setFlag(OPT_DCE);
  optimizations.setFlag(OPT_FP_OMISSION);
  optimizations.setFlag(OPT_STACK_RESERVE);
  optimizations.setFlag(OPT_CONSTANT_PROPAGATION);
}

HCC::~HCC() {
  if (outFd)
    fclose(outFd);
  if (backend)
    delete backend;
}

Result<void, std::string> HCC::parseAndCompile() {
  yyscan_t scanner;
  YY_BUFFER_STATE buffer;

  yylex_init(&scanner);
  yyset_extra(this->parser, scanner);

  hcc_parse_error.clear();
  hccCompileError.clear();

  if (sources.empty()) {
    return Result<void, std::string>::error("no sources provided");
  }

  if (!outFd) {
    return Result<void, std::string>::error("outFd == nullptr");
  }

  if (!backend) {
    return Result<void, std::string>::error("no backend selected");
  }

  std::string code = "";
  for (std::string source : sources) {
    auto result = readFile(source);
    if (result.isError()) {
      fmt::print("[hcc] failed to read {}: {}\n", source, result.getError().value());
    }

    code += result.getSuccess().value() + "\n";
  }

  buffer = yy_scan_string(code.c_str(), scanner);
  yyparse(scanner, this->parser);

  yy_delete_buffer(buffer, scanner);
  yylex_destroy(scanner);

  if (!parser->root) {
    return Result<void, std::string>::error(fmt::format("root == nullptr (parse error: {})", hcc_parse_error));
  }

  if (printAst) {
    parser->root->print();
  }

  if (!parser->root->compile(this)) {
    return Result<void, std::string>::error("compile error: " + hccCompileError);
  }

  if (ir.resultsInError(this)) {
    return Result<void, std::string>::error("ir compile error: " + hccCompileError);
  }
  ir.performStaticOptimizations(this);
  if (!ir.compile(this)) {
    return Result<void, std::string>::error("ir compile error: " + hccCompileError);
  }

  fmt::fprintf(outFd, "%s", backend->output);

  if (parser->root)
    delete parser->root;

  return Result<void, std::string>::success();
}

void HCC::openOutput(std::string filename) {
  if (outFd)
    fclose(outFd);
  outFd = fopen(filename.c_str(), "w");
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
  return outFd;
}
