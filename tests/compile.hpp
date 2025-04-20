#pragma once
#define HCC_NOPRIVATE
#include <hcc.hpp>

extern std::string compile_output;

Result<NoSuccess, std::string> compile_quick(std::string code, std::string backend);
