#pragma once
#define HCC_NOPRIVATE
#include <hcc.hpp>

extern std::string compileOutput;

Result<void, std::string> compileQuick(std::string code, std::string backend);
