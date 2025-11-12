#pragma once
#define HCC_NOPRIVATE
#include "dep_pch.hpp"
#include <hcc.hpp>

extern std::string compile_output;

tl::expected<void, std::string> compileQuick(std::string code, std::string backend);
