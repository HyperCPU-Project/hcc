#include <backend/qproc/qproc_backend.hpp>
#include <hcc.hpp>
#include <util.hpp>
#include <value/value.hpp>

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
			fmt::print("[ncc] failed to read {}: {}\n", source, result.get_error().value());
		}

		code += result.get_success().value() + "\n";
	}

	buffer = yy_scan_string(code.c_str());
	yyparse();

	if (!root) {
		return Result<NoSuccess, std::string>::error("root == nullptr");
	}

	if (print_ast) {
		root->print();
	}

	root->compile(this);

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
		// TODO
	} else {
		return Result<NoSuccess, std::string>::error("no such backend");
	}

	return Result<NoSuccess, std::string>::success({});
}

FILE* HCC::getOutFd() {
	return outfd;
}
