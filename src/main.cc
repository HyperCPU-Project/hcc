#include <hcc.hpp>
#include <pch.hpp>
#include <util.hpp>

using namespace hcc;

int main(int argc, char** argv) {
	hcc::HCC hcc;

	argsShift();
	for ([[maybe_unused]] int i = 0; argc; ++i) {
		std::string arg = argsShift();

		if (arg == "--help" || arg == "-h") {
			fmt::print(R"(usage: hcc [OPTION]... [INPUT]
options:
  --help | -h      print this message
  -o               set output filename
  --backend        set a backend
	--ast           print AST
backends:
  qproc
  hypercpu (beta)
)");
			return 0;
		} else if (arg == "-o") {
			hcc.output_filename = argsShift();
		} else if (arg == "--ast") {
			hcc.printAst = true;
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
			return 1;
		}
	}

	return 0;
}
