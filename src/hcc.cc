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

	for (IPlugin* plugin : plugins) {
		delete plugin;
	}

	for (void* handle : plugins_dlhandles) {
		dlclose(handle);
	}
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
			fmt::print("[ncc] failed to read {}: {}\n", source, result.get_error().value());
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

Result<NoSuccess, std::string> HCC::loadPlugin(std::string filename) {
	IPlugin* plugin = nullptr;

	void* handle = dlopen(filename.c_str(), RTLD_NOW);
	if (!handle) {
		return Result<NoSuccess, std::string>::error(std::format("failed to open plugin file: {}", dlerror()));
	}

	dlerror();

	hccplugin_create_t hccplugin_create = (hccplugin_create_t)(dlsym(handle, "hccplugin_create"));
	if (!hccplugin_create) {
		dlclose(handle);
		return Result<NoSuccess, std::string>::error("could not find symbol hccplugin_create");
	}

	plugin = hccplugin_create();
	if (!plugin) {
		dlclose(handle);
		return Result<NoSuccess, std::string>::error("hccplugin_create possibly failed");
	}

	plugins_dlhandles.push_back(handle);
	plugins.push_back(plugin);

	return Result<NoSuccess, std::string>::success({});
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
