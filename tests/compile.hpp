#pragma once
#include "dep_pch.hpp"
#include <hcc.hpp>

tl::expected<std::string, std::string> compileQuick(std::string code, std::string backend);
