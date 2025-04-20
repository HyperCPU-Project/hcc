#include <ast/ast.hpp>
#include <hcc.hpp>
#include <pch.hpp>
#include <util.hpp>
#include <value/value.hpp>
#include <yy.hpp>

using namespace hcc;

int main(int argc, char** argv) {
	HCC hcc;

	hcc.openOutput("a.s");

	argsShift();
	for ([[maybe_unused]] int i = 0; argc; ++i) {
		std::string arg = argsShift();

		if (arg == "--help" || arg == "-h") {
			fmt::print(R"(usage: hcc [OPTION]... [INPUT]
options:
  --help | -h      print this message
  -o               set output filename
  --backend        set a backend
  --ast            print AST
backends:
  qproc
  hypercpu (beta)
)");
			return 0;
		} else if (arg == "-o") {
			hcc.openOutput(argsShift());
		} else if (arg == "--ast") {
			hcc.print_ast = true;
		} else if (arg == "--backend") {
			auto result = hcc.selectBackend(argsShift());
			if (result.is_error()) {
				fmt::print("[hcc] failed to select a backend: {}\n", result.get_error().value());
				return 1;
			}
		} else {
			hcc.sources.push_back(arg);
		}
	}

	{
		auto result = hcc.parseAndCompile();
		if (result.is_error()) {
			fmt::print("[hcc] error: {}\n", result.get_error().value());
		}
	}

	return 0;
}
