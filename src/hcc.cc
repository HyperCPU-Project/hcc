#include <hcc.hpp>
#include <util.hpp>

using namespace hcc;

HCC::HCC() : outfd(nullptr), print_ast(false) {
}

Result<NoSuccess, std::string> HCC::parseAndCompile() {
	if (sources.empty()) {
		return Result<NoSuccess, std::string>::error("no sources provided");
	}

	if (!outfd) {
		return Result<NoSuccess, std::string>::error("outfd == nullptr");
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

	if (print_ast) {
		root->print();
	}

	// TODO: compile

	if (root) {
		root->print();
		delete root;
	}

	return Result<NoSuccess, std::string>::success({});
}

void HCC::openOutput(std::string filename) {
	if (outfd)
		fclose(outfd);
	outfd = fopen(filename.c_str(), "w");
}