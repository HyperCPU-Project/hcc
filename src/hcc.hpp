#pragma once
#include <backend/backend.hpp>
#include <pch.hpp>
#include <plugin/hcc_plugin.hpp>
#include <result.hpp>
#include <yy.hpp>

#ifdef HCC_NOPRIVATE
#define hccprivate public
#else
#define hccprivate private
#endif

extern std::string hcc_compile_error;

namespace hcc {
class Value;

class HCC {
	hccprivate : YY_BUFFER_STATE buffer;
	FILE* outfd;

	std::vector<void*> plugins_dlhandles = {};

public:
	std::vector<IPlugin*> plugins = {};
	std::vector<std::string> sources;
	bool print_ast;

	Backend* backend;

	FunctionMetadata current_function;
	std::stack<std::unique_ptr<Value>> values;

	HCC();
	~HCC();

	Result<NoSuccess, std::string> parseAndCompile();

	void openOutput(std::string filename);
	Result<NoSuccess, std::string> loadPlugin(std::string filename);

	Result<NoSuccess, std::string> selectBackend(std::string name);

	FILE* getOutFd();
};

} // namespace hcc
