#pragma once
#include <backend/backend.hpp>
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

	Backend* backend;

	HCC();

	Result<NoSuccess, std::string> parseAndCompile();

	void openOutput(std::string filename);
	Result<NoSuccess, std::string> selectBackend(std::string name);

	FILE* getOutFd();
};

} // namespace hcc