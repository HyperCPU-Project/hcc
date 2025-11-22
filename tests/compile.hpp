#pragma once
#define HCC_NOPRIVATE
#include "dep_pch.hpp"
#include <hcc.hpp>

tl::expected<std::string, std::string> compileQuick(std::string code, std::string backend);
