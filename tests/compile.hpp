#pragma once
#define HCC_NOPRIVATE
#include <hcc.hpp>

extern std::string compile_output;

Result<void, std::string> compileQuick(std::string code, std::string backend);
