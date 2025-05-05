#include <backend/hypercpu/hypercpu_backend.hpp>
#include <backend/qproc/qproc_backend.hpp>
#include <fmt/printf.h>
#include <hcc.hpp>
#include <util.hpp>
#include <value/value.hpp>
#include <yy.hpp>

std::string hcc_compile_error = "";

using namespace hcc;

HCC::HCC() : outfd(nullptr), print_ast(false), backend(nullptr), values() {
	current_function.align = 0;
}

HCC::~HCC() {
	if (outfd)
		fclose(outfd);
	if (backend)
		delete backend;
}

Result<NoSuccess, std::string> HCC::parseAndCompile() {
	hcc_parse_error.clear();
	hcc_compile_error.clear();
	line_num = 1;
	root = nullptr;

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

	buffer = yy_scan_string(code.c_str());
	yyparse();
	yy_delete_buffer(buffer);

	if (!root) {
		return Result<NoSuccess, std::string>::error(fmt::format("root == nullptr (parse error: {})", hcc_parse_error));
	}

	if (print_ast) {
		root->print();
	}

	if (!root->compile(this)) {
		return Result<NoSuccess, std::string>::error("compile error: " + hcc_compile_error);
	}

	fmt::fprintf(outfd, "%s", backend->output);

	if (root)
		delete root;

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

FILE* HCC::getOutFd() {
	return outfd;
}
