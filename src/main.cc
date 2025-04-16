#include <ncc.hpp>
#include <pch.hpp>
#include <util.hpp>

int main(int argc, char** argv) {
	NCC ncc;

	argsShift();
	for ([[maybe_unused]] int i = 0; argc; ++i) {
		std::string arg = argsShift();

		if (arg == "--help" || arg == "-h") {
			fmt::print(R"(usage: ncc [OPTION]... [INPUT]
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
			ncc.output_filename = argsShift();
		} else if (arg == "--ast") {
			ncc.printAst = true;
		} else if (arg == "--backend") {
			auto result = ncc.selectBackend(argsShift());
			if (result.is_error()) {
				fmt::print("[ncc] failed to select a backend: {}\n", result.get_error().value());
				return 1;
			}
		} else {
			ncc.sources.push_back(arg);
		}
	}

	{
		auto result = ncc.parseAndCompile();
		if (result.is_error()) {
			fmt::print("[ncc] error: {}\n", result.get_error().value());
			return 1;
		}
	}

	return 0;
}
