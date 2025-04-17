#pragma once
#include <pch.hpp>
#include <result.hpp>
#include <yy.hpp>

namespace hcc {

class HCC {
private:
	YY_BUFFER_STATE buffer;
	FILE* outfd;

public:
	std::vector<std::string> sources;
	bool print_ast;

	HCC();

	Result<NoSuccess, std::string> parseAndCompile();
	void openOutput(std::string filename);
};

} // namespace hcc