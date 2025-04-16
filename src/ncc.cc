#include <backend/backend.hpp>
#include <backend/hypercpu/hypercpu_backend.hpp>
#include <backend/qproc/qproc_backend.hpp>
#include <ncc.hpp>
#include <util.hpp>

NCC::NCC() : parser(lexer, nullptr), backend(nullptr), printAst(false) {
	output_filename = "a.out";
}

NCC::~NCC() {
	if (backend) {
		delete backend;
	}
}

Result<NoSuccess, std::string> NCC::parseAndCompile() {
	if (sources.empty()) {
		return Result<NoSuccess, std::string>::error("No sources provided");
	}

	if (!backend) {
		return Result<NoSuccess, std::string>::error("No backend selected");
	}

	std::string code;
	assembly_output = "";

	for (std::string& source : sources) {
		auto result = readFile(source);
		if (result.is_error()) {
			fmt::print("[ncc] failed to read {}: {}\n", source, result.get_error().value());
		}

		code += result.get_success().value() + "\n";
	}

	lexer.code = code;

	fmt::print("[ncc] parsing\n");
	if (!parser.parse())
		return Result<NoSuccess, std::string>::error("parser error");

	fmt::print("[ncc] compiling\n");
	compile();

	std::ofstream outfd;
	outfd.open(output_filename);
	outfd << assembly_output;
	outfd.close();

	fmt::print("[ncc] assembly written to {}\n", output_filename);

	return Result<NoSuccess, std::string>::success({});
}

Result<NoSuccess, std::string> NCC::selectBackend(std::string backend_name) {
	if (backend != nullptr) {
		return Result<NoSuccess, std::string>::error("Backend already selected");
	}

	if (backend_name == "qproc") {
		backend = new QprocBackend();
		parser.backend = backend;
		return Result<NoSuccess, std::string>::success({});
	} else if (backend_name == "hypercpu") {
		backend = new HyperCPUBackend();
		parser.backend = backend;
		return Result<NoSuccess, std::string>::success({});
	}

	return Result<NoSuccess, std::string>::error("Unknown backend");
}