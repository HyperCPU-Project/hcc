#pragma once
#define HCC_NOPRIVATE
#include <hcc.hpp>

extern std::string compile_output;

std::optional<std::string> compileQuick(std::string code, std::string backend);
